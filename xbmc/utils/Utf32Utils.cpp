/*
 *      Copyright (C) 2013 Team XBMC
 *      http://xbmc.org
 *
 *  This Program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2, or (at your option)
 *  any later version.
 *
 *  This Program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with XBMC; see the file COPYING.  If not, see
 *  <http://www.gnu.org/licenses/>.
 *
 */

#include <cassert>
#include <locale>
#include <algorithm>
#include "Utf32Utils.h"
#include "utils/CharsetConverter.h"

const CUtf32Utils::digitsMap CUtf32Utils::m_digitsMap(CUtf32Utils::digitsMapFiller());
const CUtf32Utils::charcharMap CUtf32Utils::m_foldSimpleCharsMap(CUtf32Utils::foldSimpleCharsMapFiller());
const CUtf32Utils::charstrMap CUtf32Utils::m_foldFullCharsMap(CUtf32Utils::foldFullCharsMapFiller());
bool CUtf32Utils::m_useTurkicCaseFolding = false;


inline bool CUtf32Utils::IsDigit(char32_t chr)
{
  double val;
  return GetDigitValue(chr, val);
}

bool CUtf32Utils::GetDigitValue(char32_t chr, double& resultValue)
{
  digitsMap::const_iterator it = m_digitsMap.find(chr);
  if (it != m_digitsMap.end())
  {
    resultValue = it->second;
    return true;
  }
  return false;
}

int CUtf32Utils::NaturalCompare(const std::u32string& left, const std::u32string right)
{
  const char32_t* const leftC = left.c_str();
  const char32_t* const rightC = right.c_str();
  const size_t leftLen = left.length();
  const size_t rightLen = right.length();
  const size_t shortestLen = std::min(leftLen, rightLen);
  
  const std::collate<wchar_t>& cl = std::use_facet< std::collate<wchar_t> >(std::locale());

  for (size_t pos = 0; pos < shortestLen; pos++)
  {
    if (leftC[pos] != rightC[pos])
    {
      double valL, valR;
      if (GetDigitValue(leftC[pos], valL) && GetDigitValue(rightC[pos], valR))
      {
        // FIXME: improve comparison of non-Arabic (and non-Arabic-Indic) numbers
        size_t posL = pos, posR = pos;
        bool leftIsDigit = true;  // initial value
        bool rightIsDigit = true; // initial value

        // check for non-zero digits before current position "pos"
        size_t lookBackPos = pos;
        while (lookBackPos-- > 0)
        {
          double digitVal;
          // "left" and "right" are same before "pos", no need to check them both
          if (!GetDigitValue(leftC[lookBackPos], digitVal))
          { // no digits or only zero digits before "pos"
            // skip all leading zero digits in "left"
            while (leftIsDigit && valL == 0) 
              leftIsDigit = GetDigitValue(leftC[++posL], valL);
            // skip all leading zero digits in "right"
            while (rightIsDigit && valR == 0)
              rightIsDigit = GetDigitValue(rightC[++posR], valR);
            break;
          }
          if (digitVal != 0)
            break; // non-zero digit is found before "pos", no need to skip leading zeros
        }

        const size_t numberOfLeadZerosL = posL - pos; // "pos" is pointing to first different (by code) digit
        const size_t numberOfLeadZerosR = posR - pos; // "pos" is pointing to first different (by code) digit

        // check if "left" and "right" have any digits after leading zeros
        if (leftIsDigit && rightIsDigit)
        {
          double numCompare = 0; // compare fist non-zero different digits
          do
          {
            if (numCompare == 0)
              numCompare = valL - valR;
            leftIsDigit = GetDigitValue(leftC[++posL], valL);
            rightIsDigit = GetDigitValue(rightC[++posR], valR);
          } while (leftIsDigit && rightIsDigit);

          if (leftIsDigit)
            return 1;   // "left" has more digits (excluding leading zeros), assuming that "left" is greater than "right"
          else if (rightIsDigit)
            return -1;  // "right" has more digits (excluding leading zeros), assuming that "right" is greater than "left"
          
          // "left" and "right" have equal number of digits (excluding leading zeros),
          if (numCompare != 0)
            return numCompare > 0 ? 1 : -1; // assuming that result is determined by first non-zero different digits

          // excluding leading zeros, "left" and "right" have equal number of digits, and all digits have same values
        }
        else if (leftIsDigit)
          return valL > 0 ? 1 : -1; // "right" has only zeros and "left" contains non-zero
        else if (rightIsDigit)
          return valR > 0 ? -1 : 1; // "left" has only zeros and "right" contains non-zero

        if (numberOfLeadZerosL > numberOfLeadZerosR)
          return -1; // "left" has more leading zeros, assume "left" is lower
        else if (numberOfLeadZerosL < numberOfLeadZerosR)
          return -1; // "right" has more leading zeros, assume "right" is lower

        // "left" and "right" have equal number of digits, and all digits have same values
        assert(posL == posR);
      }
      const std::wstring leftWChr(g_charsetConverter.charUtf32toWstr(leftC[pos]));   // wstring representation of "left[pos]"
      const std::wstring rightWChr(g_charsetConverter.charUtf32toWstr(rightC[pos])); // wstring representation of "right[pos]"
      const wchar_t* const leftWChrC = leftWChr.c_str();
      const wchar_t* const rightWChrC = rightWChr.c_str();
      const int compareRsl = cl.compare(leftWChrC, leftWChrC + leftWChr.length(), rightWChrC, rightWChrC + rightWChr.length());
      if (compareRsl != 0)
        return compareRsl;
      else
      { // bad conversion to wstring?
        // fallback: compare characters by code
        if (leftC[pos] > rightC[pos])
          return 1;
        else if (leftC[pos] < rightC[pos])
          return -1;
      }
    }
  }

  if (leftLen > rightLen)
    return 1;
  else if (leftLen < rightLen)
    return -1;

  return 0; // strings are equal
}

std::u32string CUtf32Utils::FoldCase(const std::u32string& str)
{
  const char32_t* const strC = str.c_str();
  const size_t len = str.length();

  std::u32string result;
  result.reserve(len); // rough estimate
  
  for (size_t pos = 0; pos < len; pos++)
  {
    const char32_t chr = strC[pos];
    if (m_useTurkicCaseFolding && (chr == 0x49 || chr == 0x130))
    {
      if (chr == 0x49)
        result.push_back(0x131);
      else // chr == 0x130
        result.push_back(0x69);
    }
    else
    {
      const charcharMap::const_iterator its = m_foldSimpleCharsMap.find(chr);
      if (its != m_foldSimpleCharsMap.end())
        result.push_back(its->second);
      else
      {
        const charstrMap::const_iterator itf = m_foldFullCharsMap.find(chr);
        if (itf != m_foldFullCharsMap.end())
          result.append(itf->second.str, itf->second.len);
        else
          result.push_back(chr);
      }
    }
  }

  return result;
}

inline std::u32string CUtf32Utils::FoldCase(const char32_t chr)
{
  if (m_useTurkicCaseFolding)
  {
    if (chr == 0x49)
      return std::u32string(1, 0x131);
    if (chr == 0x130)
      return std::u32string(1, 0x69);
  }

  const charcharMap::const_iterator its = m_foldSimpleCharsMap.find(chr);
  if (its != m_foldSimpleCharsMap.end())
    return std::u32string(1, its->second);

  const charstrMap::const_iterator itf = m_foldFullCharsMap.find(chr);
  if (itf != m_foldFullCharsMap.end())
    return std::u32string(itf->second.str, itf->second.len);

  return std::u32string(1, chr);
}


CUtf32Utils::digitsMap CUtf32Utils::digitsMapFiller(void)
{
  digitsMap m;

  // source: DerivedNumericValues-6.3.0.txt
  // see ftp://ftp.unicode.org/Public/6.3.0/ucd/extracted/DerivedNumericValues.txt

  m.insert(digitsMapElement( 0x0F33 , -1.0/2.0 )); // No       TIBETAN DIGIT HALF ZERO

  m.insert(digitsMapElement( 0x0030 , 0 )); // Nd       DIGIT ZERO
  m.insert(digitsMapElement( 0x0660 , 0 )); // Nd       ARABIC-INDIC DIGIT ZERO
  m.insert(digitsMapElement( 0x06F0 , 0 )); // Nd       EXTENDED ARABIC-INDIC DIGIT ZERO
  m.insert(digitsMapElement( 0x07C0 , 0 )); // Nd       NKO DIGIT ZERO
  m.insert(digitsMapElement( 0x0966 , 0 )); // Nd       DEVANAGARI DIGIT ZERO
  m.insert(digitsMapElement( 0x09E6 , 0 )); // Nd       BENGALI DIGIT ZERO
  m.insert(digitsMapElement( 0x0A66 , 0 )); // Nd       GURMUKHI DIGIT ZERO
  m.insert(digitsMapElement( 0x0AE6 , 0 )); // Nd       GUJARATI DIGIT ZERO
  m.insert(digitsMapElement( 0x0B66 , 0 )); // Nd       ORIYA DIGIT ZERO
  m.insert(digitsMapElement( 0x0BE6 , 0 )); // Nd       TAMIL DIGIT ZERO
  m.insert(digitsMapElement( 0x0C66 , 0 )); // Nd       TELUGU DIGIT ZERO
  m.insert(digitsMapElement( 0x0C78 , 0 )); // No       TELUGU FRACTION DIGIT ZERO FOR ODD POWERS OF FOUR
  m.insert(digitsMapElement( 0x0CE6 , 0 )); // Nd       KANNADA DIGIT ZERO
  m.insert(digitsMapElement( 0x0D66 , 0 )); // Nd       MALAYALAM DIGIT ZERO
  m.insert(digitsMapElement( 0x0E50 , 0 )); // Nd       THAI DIGIT ZERO
  m.insert(digitsMapElement( 0x0ED0 , 0 )); // Nd       LAO DIGIT ZERO
  m.insert(digitsMapElement( 0x0F20 , 0 )); // Nd       TIBETAN DIGIT ZERO
  m.insert(digitsMapElement( 0x1040 , 0 )); // Nd       MYANMAR DIGIT ZERO
  m.insert(digitsMapElement( 0x1090 , 0 )); // Nd       MYANMAR SHAN DIGIT ZERO
  m.insert(digitsMapElement( 0x17E0 , 0 )); // Nd       KHMER DIGIT ZERO
  m.insert(digitsMapElement( 0x17F0 , 0 )); // No       KHMER SYMBOL LEK ATTAK SON
  m.insert(digitsMapElement( 0x1810 , 0 )); // Nd       MONGOLIAN DIGIT ZERO
  m.insert(digitsMapElement( 0x1946 , 0 )); // Nd       LIMBU DIGIT ZERO
  m.insert(digitsMapElement( 0x19D0 , 0 )); // Nd       NEW TAI LUE DIGIT ZERO
  m.insert(digitsMapElement( 0x1A80 , 0 )); // Nd       TAI THAM HORA DIGIT ZERO
  m.insert(digitsMapElement( 0x1A90 , 0 )); // Nd       TAI THAM THAM DIGIT ZERO
  m.insert(digitsMapElement( 0x1B50 , 0 )); // Nd       BALINESE DIGIT ZERO
  m.insert(digitsMapElement( 0x1BB0 , 0 )); // Nd       SUNDANESE DIGIT ZERO
  m.insert(digitsMapElement( 0x1C40 , 0 )); // Nd       LEPCHA DIGIT ZERO
  m.insert(digitsMapElement( 0x1C50 , 0 )); // Nd       OL CHIKI DIGIT ZERO
  m.insert(digitsMapElement( 0x2070 , 0 )); // No       SUPERSCRIPT ZERO
  m.insert(digitsMapElement( 0x2080 , 0 )); // No       SUBSCRIPT ZERO
  m.insert(digitsMapElement( 0x2189 , 0 )); // No       VULGAR FRACTION ZERO THIRDS
  m.insert(digitsMapElement( 0x24EA , 0 )); // No       CIRCLED DIGIT ZERO
  m.insert(digitsMapElement( 0x24FF , 0 )); // No       NEGATIVE CIRCLED DIGIT ZERO
  m.insert(digitsMapElement( 0x3007 , 0 )); // Nl       IDEOGRAPHIC NUMBER ZERO
  m.insert(digitsMapElement( 0x96F6 , 0 )); // Lo       CJK UNIFIED IDEOGRAPH-96F6
  m.insert(digitsMapElement( 0xA620 , 0 )); // Nd       VAI DIGIT ZERO
  m.insert(digitsMapElement( 0xA6EF , 0 )); // Nl       BAMUM LETTER KOGHOM
  m.insert(digitsMapElement( 0xA8D0 , 0 )); // Nd       SAURASHTRA DIGIT ZERO
  m.insert(digitsMapElement( 0xA900 , 0 )); // Nd       KAYAH LI DIGIT ZERO
  m.insert(digitsMapElement( 0xA9D0 , 0 )); // Nd       JAVANESE DIGIT ZERO
  m.insert(digitsMapElement( 0xAA50 , 0 )); // Nd       CHAM DIGIT ZERO
  m.insert(digitsMapElement( 0xABF0 , 0 )); // Nd       MEETEI MAYEK DIGIT ZERO
  m.insert(digitsMapElement( 0xF9B2 , 0 )); // Lo       CJK COMPATIBILITY IDEOGRAPH-F9B2
  m.insert(digitsMapElement( 0xFF10 , 0 )); // Nd       FULLWIDTH DIGIT ZERO
  m.insert(digitsMapElement( 0x1018A, 0 )); // No       GREEK ZERO SIGN
  m.insert(digitsMapElement( 0x104A0, 0 )); // Nd       OSMANYA DIGIT ZERO
  m.insert(digitsMapElement( 0x11066, 0 )); // Nd       BRAHMI DIGIT ZERO
  m.insert(digitsMapElement( 0x110F0, 0 )); // Nd       SORA SOMPENG DIGIT ZERO
  m.insert(digitsMapElement( 0x11136, 0 )); // Nd       CHAKMA DIGIT ZERO
  m.insert(digitsMapElement( 0x111D0, 0 )); // Nd       SHARADA DIGIT ZERO
  m.insert(digitsMapElement( 0x116C0, 0 )); // Nd       TAKRI DIGIT ZERO
  m.insert(digitsMapElement( 0x1D7CE, 0 )); // Nd       MATHEMATICAL BOLD DIGIT ZERO
  m.insert(digitsMapElement( 0x1D7D8, 0 )); // Nd       MATHEMATICAL DOUBLE-STRUCK DIGIT ZERO
  m.insert(digitsMapElement( 0x1D7E2, 0 )); // Nd       MATHEMATICAL SANS-SERIF DIGIT ZERO
  m.insert(digitsMapElement( 0x1D7EC, 0 )); // Nd       MATHEMATICAL SANS-SERIF BOLD DIGIT ZERO
  m.insert(digitsMapElement( 0x1D7F6, 0 )); // Nd       MATHEMATICAL MONOSPACE DIGIT ZERO
  m.insert(digitsMapElement( 0x1F100, 0 )); // No   [2] DIGIT ZERO FULL STOP..DIGIT ZERO COMMA
  m.insert(digitsMapElement( 0x1F101, 0 )); // No   [2] DIGIT ZERO FULL STOP..DIGIT ZERO COMMA

  m.insert(digitsMapElement( 0x09F4 , 1.0/16.0 )); // No       BENGALI CURRENCY NUMERATOR ONE
  m.insert(digitsMapElement( 0x0B75 , 1.0/16.0 )); // No       ORIYA FRACTION ONE SIXTEENTH
  m.insert(digitsMapElement( 0xA833 , 1.0/16.0 )); // No       NORTH INDIC FRACTION ONE SIXTEENTH

  m.insert(digitsMapElement( 0x2152 , 1.0/10.0 )); // No       VULGAR FRACTION ONE TENTH

  m.insert(digitsMapElement( 0x2151 , 1.0/9.0 )); // No       VULGAR FRACTION ONE NINTH

  m.insert(digitsMapElement( 0x09F5 , 1.0/8.0 )); // No       BENGALI CURRENCY NUMERATOR TWO
  m.insert(digitsMapElement( 0x0B76 , 1.0/8.0 )); // No       ORIYA FRACTION ONE EIGHTH
  m.insert(digitsMapElement( 0x215B , 1.0/8.0 )); // No       VULGAR FRACTION ONE EIGHTH
  m.insert(digitsMapElement( 0xA834 , 1.0/8.0 )); // No       NORTH INDIC FRACTION ONE EIGHTH
  m.insert(digitsMapElement( 0x1245F, 1.0/8.0 )); // Nl       CUNEIFORM NUMERIC SIGN ONE EIGHTH ASH

  m.insert(digitsMapElement( 0x2150 , 1.0/7.0 )); // No       VULGAR FRACTION ONE SEVENTH

  m.insert(digitsMapElement( 0x2159 , 1.0/6.0 )); // No       VULGAR FRACTION ONE SIXTH
  m.insert(digitsMapElement( 0x12461, 1.0/6.0 )); // Nl       CUNEIFORM NUMERIC SIGN OLD ASSYRIAN ONE SIXTH

  m.insert(digitsMapElement( 0x09F6 , 3.0/16.0 )); // No       BENGALI CURRENCY NUMERATOR THREE
  m.insert(digitsMapElement( 0x0B77 , 3.0/16.0 )); // No       ORIYA FRACTION THREE SIXTEENTHS
  m.insert(digitsMapElement( 0xA835 , 3.0/16.0 )); // No       NORTH INDIC FRACTION THREE SIXTEENTHS

  m.insert(digitsMapElement( 0x2155 , 1.0/5.0 )); // No       VULGAR FRACTION ONE FIFTH

  m.insert(digitsMapElement( 0x00BC , 1.0/4.0 )); // No       VULGAR FRACTION ONE QUARTER
  m.insert(digitsMapElement( 0x09F7 , 1.0/4.0 )); // No       BENGALI CURRENCY NUMERATOR FOUR
  m.insert(digitsMapElement( 0x0B72 , 1.0/4.0 )); // No       ORIYA FRACTION ONE QUARTER
  m.insert(digitsMapElement( 0x0D73 , 1.0/4.0 )); // No       MALAYALAM FRACTION ONE QUARTER
  m.insert(digitsMapElement( 0xA830 , 1.0/4.0 )); // No       NORTH INDIC FRACTION ONE QUARTER
  m.insert(digitsMapElement( 0x10140, 1.0/4.0 )); // Nl       GREEK ACROPHONIC ATTIC ONE QUARTER
  m.insert(digitsMapElement( 0x10E7C, 1.0/4.0 )); // No       RUMI FRACTION ONE QUARTER
  m.insert(digitsMapElement( 0x12460, 1.0/4.0 )); // Nl       CUNEIFORM NUMERIC SIGN ONE QUARTER ASH
  m.insert(digitsMapElement( 0x12462, 1.0/4.0 )); // Nl       CUNEIFORM NUMERIC SIGN OLD ASSYRIAN ONE QUARTER

  m.insert(digitsMapElement( 0x2153 , 1.0/3.0 )); // No       VULGAR FRACTION ONE THIRD
  m.insert(digitsMapElement( 0x10E7D, 1.0/3.0 )); // No       RUMI FRACTION ONE THIRD
  m.insert(digitsMapElement( 0x1245A, 1.0/3.0 )); // Nl       CUNEIFORM NUMERIC SIGN ONE THIRD DISH
  m.insert(digitsMapElement( 0x1245D, 1.0/3.0 )); // Nl       CUNEIFORM NUMERIC SIGN ONE THIRD VARIANT FORM A

  m.insert(digitsMapElement( 0x215C , 3.0/8.0 )); // No       VULGAR FRACTION THREE EIGHTHS

  m.insert(digitsMapElement( 0x2156 , 2.0/5.0 )); // No       VULGAR FRACTION TWO FIFTHS

  m.insert(digitsMapElement( 0x00BD , 1.0/2.0 )); // No       VULGAR FRACTION ONE HALF
  m.insert(digitsMapElement( 0x0B73 , 1.0/2.0 )); // No       ORIYA FRACTION ONE HALF
  m.insert(digitsMapElement( 0x0D74 , 1.0/2.0 )); // No       MALAYALAM FRACTION ONE HALF
  m.insert(digitsMapElement( 0x0F2A , 1.0/2.0 )); // No       TIBETAN DIGIT HALF ONE
  m.insert(digitsMapElement( 0x2CFD , 1.0/2.0 )); // No       COPTIC FRACTION ONE HALF
  m.insert(digitsMapElement( 0xA831 , 1.0/2.0 )); // No       NORTH INDIC FRACTION ONE HALF
  m.insert(digitsMapElement( 0x10141, 1.0/2.0 )); // Nl       GREEK ACROPHONIC ATTIC ONE HALF
  m.insert(digitsMapElement( 0x10175, 1.0/2.0 )); // No   [2] GREEK ONE HALF SIGN..GREEK ONE HALF SIGN ALTERNATE FORM
  m.insert(digitsMapElement( 0x10176, 1.0/2.0 )); // No   [2] GREEK ONE HALF SIGN..GREEK ONE HALF SIGN ALTERNATE FORM
  m.insert(digitsMapElement( 0x10E7B, 1.0/2.0 )); // No       RUMI FRACTION ONE HALF

  m.insert(digitsMapElement( 0x2157 , 3.0/5.0 )); // No       VULGAR FRACTION THREE FIFTHS

  m.insert(digitsMapElement( 0x215D , 5.0/8.0 )); // No       VULGAR FRACTION FIVE EIGHTHS

  m.insert(digitsMapElement( 0x2154 , 2.0/3.0 )); // No       VULGAR FRACTION TWO THIRDS
  m.insert(digitsMapElement( 0x10177, 2.0/3.0 )); // No       GREEK TWO THIRDS SIGN
  m.insert(digitsMapElement( 0x10E7E, 2.0/3.0 )); // No       RUMI FRACTION TWO THIRDS
  m.insert(digitsMapElement( 0x1245B, 2.0/3.0 )); // Nl       CUNEIFORM NUMERIC SIGN TWO THIRDS DISH
  m.insert(digitsMapElement( 0x1245E, 2.0/3.0 )); // Nl       CUNEIFORM NUMERIC SIGN TWO THIRDS VARIANT FORM A

  m.insert(digitsMapElement( 0x00BE , 3.0/4.0 )); // No       VULGAR FRACTION THREE QUARTERS
  m.insert(digitsMapElement( 0x09F8 , 3.0/4.0 )); // No       BENGALI CURRENCY NUMERATOR ONE LESS THAN THE DENOMINATOR
  m.insert(digitsMapElement( 0x0B74 , 3.0/4.0 )); // No       ORIYA FRACTION THREE QUARTERS
  m.insert(digitsMapElement( 0x0D75 , 3.0/4.0 )); // No       MALAYALAM FRACTION THREE QUARTERS
  m.insert(digitsMapElement( 0xA832 , 3.0/4.0 )); // No       NORTH INDIC FRACTION THREE QUARTERS
  m.insert(digitsMapElement( 0x10178, 3.0/4.0 )); // No       GREEK THREE QUARTERS SIGN

  m.insert(digitsMapElement( 0x2158 , 4.0/5.0 )); // No       VULGAR FRACTION FOUR FIFTHS

  m.insert(digitsMapElement( 0x215A , 5.0/6.0 )); // No       VULGAR FRACTION FIVE SIXTHS
  m.insert(digitsMapElement( 0x1245C, 5.0/6.0 )); // Nl       CUNEIFORM NUMERIC SIGN FIVE SIXTHS DISH

  m.insert(digitsMapElement( 0x215E , 7.0/8.0 )); // No       VULGAR FRACTION SEVEN EIGHTHS

  m.insert(digitsMapElement( 0x0031 , 1 )); // Nd       DIGIT ONE
  m.insert(digitsMapElement( 0x00B9 , 1 )); // No       SUPERSCRIPT ONE
  m.insert(digitsMapElement( 0x0661 , 1 )); // Nd       ARABIC-INDIC DIGIT ONE
  m.insert(digitsMapElement( 0x06F1 , 1 )); // Nd       EXTENDED ARABIC-INDIC DIGIT ONE
  m.insert(digitsMapElement( 0x07C1 , 1 )); // Nd       NKO DIGIT ONE
  m.insert(digitsMapElement( 0x0967 , 1 )); // Nd       DEVANAGARI DIGIT ONE
  m.insert(digitsMapElement( 0x09E7 , 1 )); // Nd       BENGALI DIGIT ONE
  m.insert(digitsMapElement( 0x0A67 , 1 )); // Nd       GURMUKHI DIGIT ONE
  m.insert(digitsMapElement( 0x0AE7 , 1 )); // Nd       GUJARATI DIGIT ONE
  m.insert(digitsMapElement( 0x0B67 , 1 )); // Nd       ORIYA DIGIT ONE
  m.insert(digitsMapElement( 0x0BE7 , 1 )); // Nd       TAMIL DIGIT ONE
  m.insert(digitsMapElement( 0x0C67 , 1 )); // Nd       TELUGU DIGIT ONE
  m.insert(digitsMapElement( 0x0C79 , 1 )); // No       TELUGU FRACTION DIGIT ONE FOR ODD POWERS OF FOUR
  m.insert(digitsMapElement( 0x0C7C , 1 )); // No       TELUGU FRACTION DIGIT ONE FOR EVEN POWERS OF FOUR
  m.insert(digitsMapElement( 0x0CE7 , 1 )); // Nd       KANNADA DIGIT ONE
  m.insert(digitsMapElement( 0x0D67 , 1 )); // Nd       MALAYALAM DIGIT ONE
  m.insert(digitsMapElement( 0x0E51 , 1 )); // Nd       THAI DIGIT ONE
  m.insert(digitsMapElement( 0x0ED1 , 1 )); // Nd       LAO DIGIT ONE
  m.insert(digitsMapElement( 0x0F21 , 1 )); // Nd       TIBETAN DIGIT ONE
  m.insert(digitsMapElement( 0x1041 , 1 )); // Nd       MYANMAR DIGIT ONE
  m.insert(digitsMapElement( 0x1091 , 1 )); // Nd       MYANMAR SHAN DIGIT ONE
  m.insert(digitsMapElement( 0x1369 , 1 )); // No       ETHIOPIC DIGIT ONE
  m.insert(digitsMapElement( 0x17E1 , 1 )); // Nd       KHMER DIGIT ONE
  m.insert(digitsMapElement( 0x17F1 , 1 )); // No       KHMER SYMBOL LEK ATTAK MUOY
  m.insert(digitsMapElement( 0x1811 , 1 )); // Nd       MONGOLIAN DIGIT ONE
  m.insert(digitsMapElement( 0x1947 , 1 )); // Nd       LIMBU DIGIT ONE
  m.insert(digitsMapElement( 0x19D1 , 1 )); // Nd       NEW TAI LUE DIGIT ONE
  m.insert(digitsMapElement( 0x19DA , 1 )); // No       NEW TAI LUE THAM DIGIT ONE
  m.insert(digitsMapElement( 0x1A81 , 1 )); // Nd       TAI THAM HORA DIGIT ONE
  m.insert(digitsMapElement( 0x1A91 , 1 )); // Nd       TAI THAM THAM DIGIT ONE
  m.insert(digitsMapElement( 0x1B51 , 1 )); // Nd       BALINESE DIGIT ONE
  m.insert(digitsMapElement( 0x1BB1 , 1 )); // Nd       SUNDANESE DIGIT ONE
  m.insert(digitsMapElement( 0x1C41 , 1 )); // Nd       LEPCHA DIGIT ONE
  m.insert(digitsMapElement( 0x1C51 , 1 )); // Nd       OL CHIKI DIGIT ONE
  m.insert(digitsMapElement( 0x2081 , 1 )); // No       SUBSCRIPT ONE
  m.insert(digitsMapElement( 0x215F , 1 )); // No       FRACTION NUMERATOR ONE
  m.insert(digitsMapElement( 0x2160 , 1 )); // Nl       ROMAN NUMERAL ONE
  m.insert(digitsMapElement( 0x2170 , 1 )); // Nl       SMALL ROMAN NUMERAL ONE
  m.insert(digitsMapElement( 0x2460 , 1 )); // No       CIRCLED DIGIT ONE
  m.insert(digitsMapElement( 0x2474 , 1 )); // No       PARENTHESIZED DIGIT ONE
  m.insert(digitsMapElement( 0x2488 , 1 )); // No       DIGIT ONE FULL STOP
  m.insert(digitsMapElement( 0x24F5 , 1 )); // No       DOUBLE CIRCLED DIGIT ONE
  m.insert(digitsMapElement( 0x2776 , 1 )); // No       DINGBAT NEGATIVE CIRCLED DIGIT ONE
  m.insert(digitsMapElement( 0x2780 , 1 )); // No       DINGBAT CIRCLED SANS-SERIF DIGIT ONE
  m.insert(digitsMapElement( 0x278A , 1 )); // No       DINGBAT NEGATIVE CIRCLED SANS-SERIF DIGIT ONE
  m.insert(digitsMapElement( 0x3021 , 1 )); // Nl       HANGZHOU NUMERAL ONE
  m.insert(digitsMapElement( 0x3192 , 1 )); // No       IDEOGRAPHIC ANNOTATION ONE MARK
  m.insert(digitsMapElement( 0x3220 , 1 )); // No       PARENTHESIZED IDEOGRAPH ONE
  m.insert(digitsMapElement( 0x3280 , 1 )); // No       CIRCLED IDEOGRAPH ONE
  m.insert(digitsMapElement( 0x4E00 , 1 )); // Lo       CJK UNIFIED IDEOGRAPH-4E00
  m.insert(digitsMapElement( 0x58F1 , 1 )); // Lo       CJK UNIFIED IDEOGRAPH-58F1
  m.insert(digitsMapElement( 0x58F9 , 1 )); // Lo       CJK UNIFIED IDEOGRAPH-58F9
  m.insert(digitsMapElement( 0x5E7A , 1 )); // Lo       CJK UNIFIED IDEOGRAPH-5E7A
  m.insert(digitsMapElement( 0x5F0C , 1 )); // Lo       CJK UNIFIED IDEOGRAPH-5F0C
  m.insert(digitsMapElement( 0xA621 , 1 )); // Nd       VAI DIGIT ONE
  m.insert(digitsMapElement( 0xA6E6 , 1 )); // Nl       BAMUM LETTER MO
  m.insert(digitsMapElement( 0xA8D1 , 1 )); // Nd       SAURASHTRA DIGIT ONE
  m.insert(digitsMapElement( 0xA901 , 1 )); // Nd       KAYAH LI DIGIT ONE
  m.insert(digitsMapElement( 0xA9D1 , 1 )); // Nd       JAVANESE DIGIT ONE
  m.insert(digitsMapElement( 0xAA51 , 1 )); // Nd       CHAM DIGIT ONE
  m.insert(digitsMapElement( 0xABF1 , 1 )); // Nd       MEETEI MAYEK DIGIT ONE
  m.insert(digitsMapElement( 0xFF11 , 1 )); // Nd       FULLWIDTH DIGIT ONE
  m.insert(digitsMapElement( 0x10107, 1 )); // No       AEGEAN NUMBER ONE
  m.insert(digitsMapElement( 0x10142, 1 )); // Nl       GREEK ACROPHONIC ATTIC ONE DRACHMA
  m.insert(digitsMapElement( 0x10158, 1 )); // Nl   [3] GREEK ACROPHONIC HERAEUM ONE PLETHRON..GREEK ACROPHONIC HERMIONIAN ONE
  m.insert(digitsMapElement( 0x10159, 1 )); // Nl   [3] GREEK ACROPHONIC HERAEUM ONE PLETHRON..GREEK ACROPHONIC HERMIONIAN ONE
  m.insert(digitsMapElement( 0x1015A, 1 )); // Nl   [3] GREEK ACROPHONIC HERAEUM ONE PLETHRON..GREEK ACROPHONIC HERMIONIAN ONE
  m.insert(digitsMapElement( 0x10320, 1 )); // No       OLD ITALIC NUMERAL ONE
  m.insert(digitsMapElement( 0x103D1, 1 )); // Nl       OLD PERSIAN NUMBER ONE
  m.insert(digitsMapElement( 0x104A1, 1 )); // Nd       OSMANYA DIGIT ONE
  m.insert(digitsMapElement( 0x10858, 1 )); // No       IMPERIAL ARAMAIC NUMBER ONE
  m.insert(digitsMapElement( 0x10916, 1 )); // No       PHOENICIAN NUMBER ONE
  m.insert(digitsMapElement( 0x10A40, 1 )); // No       KHAROSHTHI DIGIT ONE
  m.insert(digitsMapElement( 0x10A7D, 1 )); // No       OLD SOUTH ARABIAN NUMBER ONE
  m.insert(digitsMapElement( 0x10B58, 1 )); // No       INSCRIPTIONAL PARTHIAN NUMBER ONE
  m.insert(digitsMapElement( 0x10B78, 1 )); // No       INSCRIPTIONAL PAHLAVI NUMBER ONE
  m.insert(digitsMapElement( 0x10E60, 1 )); // No       RUMI DIGIT ONE
  m.insert(digitsMapElement( 0x11052, 1 )); // No       BRAHMI NUMBER ONE
  m.insert(digitsMapElement( 0x11067, 1 )); // Nd       BRAHMI DIGIT ONE
  m.insert(digitsMapElement( 0x110F1, 1 )); // Nd       SORA SOMPENG DIGIT ONE
  m.insert(digitsMapElement( 0x11137, 1 )); // Nd       CHAKMA DIGIT ONE
  m.insert(digitsMapElement( 0x111D1, 1 )); // Nd       SHARADA DIGIT ONE
  m.insert(digitsMapElement( 0x116C1, 1 )); // Nd       TAKRI DIGIT ONE
  m.insert(digitsMapElement( 0x12415, 1 )); // Nl       CUNEIFORM NUMERIC SIGN ONE GESH2
  m.insert(digitsMapElement( 0x1241E, 1 )); // Nl       CUNEIFORM NUMERIC SIGN ONE GESHU
  m.insert(digitsMapElement( 0x1242C, 1 )); // Nl       CUNEIFORM NUMERIC SIGN ONE SHARU
  m.insert(digitsMapElement( 0x12434, 1 )); // Nl       CUNEIFORM NUMERIC SIGN ONE BURU
  m.insert(digitsMapElement( 0x1244F, 1 )); // Nl       CUNEIFORM NUMERIC SIGN ONE BAN2
  m.insert(digitsMapElement( 0x12458, 1 )); // Nl       CUNEIFORM NUMERIC SIGN ONE ESHE3
  m.insert(digitsMapElement( 0x1D360, 1 )); // No       COUNTING ROD UNIT DIGIT ONE
  m.insert(digitsMapElement( 0x1D7CF, 1 )); // Nd       MATHEMATICAL BOLD DIGIT ONE
  m.insert(digitsMapElement( 0x1D7D9, 1 )); // Nd       MATHEMATICAL DOUBLE-STRUCK DIGIT ONE
  m.insert(digitsMapElement( 0x1D7E3, 1 )); // Nd       MATHEMATICAL SANS-SERIF DIGIT ONE
  m.insert(digitsMapElement( 0x1D7ED, 1 )); // Nd       MATHEMATICAL SANS-SERIF BOLD DIGIT ONE
  m.insert(digitsMapElement( 0x1D7F7, 1 )); // Nd       MATHEMATICAL MONOSPACE DIGIT ONE
  m.insert(digitsMapElement( 0x1F102, 1 )); // No       DIGIT ONE COMMA
  m.insert(digitsMapElement( 0x2092A, 1 )); // Lo       CJK UNIFIED IDEOGRAPH-2092A

  m.insert(digitsMapElement( 0x0F2B , 3.0/2.0 )); // No       TIBETAN DIGIT HALF TWO

  m.insert(digitsMapElement( 0x0032 , 2 )); // Nd       DIGIT TWO
  m.insert(digitsMapElement( 0x00B2 , 2 )); // No       SUPERSCRIPT TWO
  m.insert(digitsMapElement( 0x0662 , 2 )); // Nd       ARABIC-INDIC DIGIT TWO
  m.insert(digitsMapElement( 0x06F2 , 2 )); // Nd       EXTENDED ARABIC-INDIC DIGIT TWO
  m.insert(digitsMapElement( 0x07C2 , 2 )); // Nd       NKO DIGIT TWO
  m.insert(digitsMapElement( 0x0968 , 2 )); // Nd       DEVANAGARI DIGIT TWO
  m.insert(digitsMapElement( 0x09E8 , 2 )); // Nd       BENGALI DIGIT TWO
  m.insert(digitsMapElement( 0x0A68 , 2 )); // Nd       GURMUKHI DIGIT TWO
  m.insert(digitsMapElement( 0x0AE8 , 2 )); // Nd       GUJARATI DIGIT TWO
  m.insert(digitsMapElement( 0x0B68 , 2 )); // Nd       ORIYA DIGIT TWO
  m.insert(digitsMapElement( 0x0BE8 , 2 )); // Nd       TAMIL DIGIT TWO
  m.insert(digitsMapElement( 0x0C68 , 2 )); // Nd       TELUGU DIGIT TWO
  m.insert(digitsMapElement( 0x0C7A , 2 )); // No       TELUGU FRACTION DIGIT TWO FOR ODD POWERS OF FOUR
  m.insert(digitsMapElement( 0x0C7D , 2 )); // No       TELUGU FRACTION DIGIT TWO FOR EVEN POWERS OF FOUR
  m.insert(digitsMapElement( 0x0CE8 , 2 )); // Nd       KANNADA DIGIT TWO
  m.insert(digitsMapElement( 0x0D68 , 2 )); // Nd       MALAYALAM DIGIT TWO
  m.insert(digitsMapElement( 0x0E52 , 2 )); // Nd       THAI DIGIT TWO
  m.insert(digitsMapElement( 0x0ED2 , 2 )); // Nd       LAO DIGIT TWO
  m.insert(digitsMapElement( 0x0F22 , 2 )); // Nd       TIBETAN DIGIT TWO
  m.insert(digitsMapElement( 0x1042 , 2 )); // Nd       MYANMAR DIGIT TWO
  m.insert(digitsMapElement( 0x1092 , 2 )); // Nd       MYANMAR SHAN DIGIT TWO
  m.insert(digitsMapElement( 0x136A , 2 )); // No       ETHIOPIC DIGIT TWO
  m.insert(digitsMapElement( 0x17E2 , 2 )); // Nd       KHMER DIGIT TWO
  m.insert(digitsMapElement( 0x17F2 , 2 )); // No       KHMER SYMBOL LEK ATTAK PII
  m.insert(digitsMapElement( 0x1812 , 2 )); // Nd       MONGOLIAN DIGIT TWO
  m.insert(digitsMapElement( 0x1948 , 2 )); // Nd       LIMBU DIGIT TWO
  m.insert(digitsMapElement( 0x19D2 , 2 )); // Nd       NEW TAI LUE DIGIT TWO
  m.insert(digitsMapElement( 0x1A82 , 2 )); // Nd       TAI THAM HORA DIGIT TWO
  m.insert(digitsMapElement( 0x1A92 , 2 )); // Nd       TAI THAM THAM DIGIT TWO
  m.insert(digitsMapElement( 0x1B52 , 2 )); // Nd       BALINESE DIGIT TWO
  m.insert(digitsMapElement( 0x1BB2 , 2 )); // Nd       SUNDANESE DIGIT TWO
  m.insert(digitsMapElement( 0x1C42 , 2 )); // Nd       LEPCHA DIGIT TWO
  m.insert(digitsMapElement( 0x1C52 , 2 )); // Nd       OL CHIKI DIGIT TWO
  m.insert(digitsMapElement( 0x2082 , 2 )); // No       SUBSCRIPT TWO
  m.insert(digitsMapElement( 0x2161 , 2 )); // Nl       ROMAN NUMERAL TWO
  m.insert(digitsMapElement( 0x2171 , 2 )); // Nl       SMALL ROMAN NUMERAL TWO
  m.insert(digitsMapElement( 0x2461 , 2 )); // No       CIRCLED DIGIT TWO
  m.insert(digitsMapElement( 0x2475 , 2 )); // No       PARENTHESIZED DIGIT TWO
  m.insert(digitsMapElement( 0x2489 , 2 )); // No       DIGIT TWO FULL STOP
  m.insert(digitsMapElement( 0x24F6 , 2 )); // No       DOUBLE CIRCLED DIGIT TWO
  m.insert(digitsMapElement( 0x2777 , 2 )); // No       DINGBAT NEGATIVE CIRCLED DIGIT TWO
  m.insert(digitsMapElement( 0x2781 , 2 )); // No       DINGBAT CIRCLED SANS-SERIF DIGIT TWO
  m.insert(digitsMapElement( 0x278B , 2 )); // No       DINGBAT NEGATIVE CIRCLED SANS-SERIF DIGIT TWO
  m.insert(digitsMapElement( 0x3022 , 2 )); // Nl       HANGZHOU NUMERAL TWO
  m.insert(digitsMapElement( 0x3193 , 2 )); // No       IDEOGRAPHIC ANNOTATION TWO MARK
  m.insert(digitsMapElement( 0x3221 , 2 )); // No       PARENTHESIZED IDEOGRAPH TWO
  m.insert(digitsMapElement( 0x3281 , 2 )); // No       CIRCLED IDEOGRAPH TWO
  m.insert(digitsMapElement( 0x3483 , 2 )); // Lo       CJK UNIFIED IDEOGRAPH-3483
  m.insert(digitsMapElement( 0x4E8C , 2 )); // Lo       CJK UNIFIED IDEOGRAPH-4E8C
  m.insert(digitsMapElement( 0x5169 , 2 )); // Lo       CJK UNIFIED IDEOGRAPH-5169
  m.insert(digitsMapElement( 0x5F0D , 2 )); // Lo       CJK UNIFIED IDEOGRAPH-5F0D
  m.insert(digitsMapElement( 0x5F10 , 2 )); // Lo       CJK UNIFIED IDEOGRAPH-5F10
  m.insert(digitsMapElement( 0x8CAE , 2 )); // Lo       CJK UNIFIED IDEOGRAPH-8CAE
  m.insert(digitsMapElement( 0x8CB3 , 2 )); // Lo       CJK UNIFIED IDEOGRAPH-8CB3
  m.insert(digitsMapElement( 0x8D30 , 2 )); // Lo       CJK UNIFIED IDEOGRAPH-8D30
  m.insert(digitsMapElement( 0xA622 , 2 )); // Nd       VAI DIGIT TWO
  m.insert(digitsMapElement( 0xA6E7 , 2 )); // Nl       BAMUM LETTER MBAA
  m.insert(digitsMapElement( 0xA8D2 , 2 )); // Nd       SAURASHTRA DIGIT TWO
  m.insert(digitsMapElement( 0xA902 , 2 )); // Nd       KAYAH LI DIGIT TWO
  m.insert(digitsMapElement( 0xA9D2 , 2 )); // Nd       JAVANESE DIGIT TWO
  m.insert(digitsMapElement( 0xAA52 , 2 )); // Nd       CHAM DIGIT TWO
  m.insert(digitsMapElement( 0xABF2 , 2 )); // Nd       MEETEI MAYEK DIGIT TWO
  m.insert(digitsMapElement( 0xF978 , 2 )); // Lo       CJK COMPATIBILITY IDEOGRAPH-F978
  m.insert(digitsMapElement( 0xFF12 , 2 )); // Nd       FULLWIDTH DIGIT TWO
  m.insert(digitsMapElement( 0x10108, 2 )); // No       AEGEAN NUMBER TWO
  m.insert(digitsMapElement( 0x1015B, 2 )); // Nl   [4] GREEK ACROPHONIC EPIDAUREAN TWO..GREEK ACROPHONIC EPIDAUREAN TWO DRACHMAS
  m.insert(digitsMapElement( 0x1015C, 2 )); // Nl   [4] GREEK ACROPHONIC EPIDAUREAN TWO..GREEK ACROPHONIC EPIDAUREAN TWO DRACHMAS
  m.insert(digitsMapElement( 0x1015D, 2 )); // Nl   [4] GREEK ACROPHONIC EPIDAUREAN TWO..GREEK ACROPHONIC EPIDAUREAN TWO DRACHMAS
  m.insert(digitsMapElement( 0x1015E, 2 )); // Nl   [4] GREEK ACROPHONIC EPIDAUREAN TWO..GREEK ACROPHONIC EPIDAUREAN TWO DRACHMAS
  m.insert(digitsMapElement( 0x103D2, 2 )); // Nl       OLD PERSIAN NUMBER TWO
  m.insert(digitsMapElement( 0x104A2, 2 )); // Nd       OSMANYA DIGIT TWO
  m.insert(digitsMapElement( 0x10859, 2 )); // No       IMPERIAL ARAMAIC NUMBER TWO
  m.insert(digitsMapElement( 0x1091A, 2 )); // No       PHOENICIAN NUMBER TWO
  m.insert(digitsMapElement( 0x10A41, 2 )); // No       KHAROSHTHI DIGIT TWO
  m.insert(digitsMapElement( 0x10B59, 2 )); // No       INSCRIPTIONAL PARTHIAN NUMBER TWO
  m.insert(digitsMapElement( 0x10B79, 2 )); // No       INSCRIPTIONAL PAHLAVI NUMBER TWO
  m.insert(digitsMapElement( 0x10E61, 2 )); // No       RUMI DIGIT TWO
  m.insert(digitsMapElement( 0x11053, 2 )); // No       BRAHMI NUMBER TWO
  m.insert(digitsMapElement( 0x11068, 2 )); // Nd       BRAHMI DIGIT TWO
  m.insert(digitsMapElement( 0x110F2, 2 )); // Nd       SORA SOMPENG DIGIT TWO
  m.insert(digitsMapElement( 0x11138, 2 )); // Nd       CHAKMA DIGIT TWO
  m.insert(digitsMapElement( 0x111D2, 2 )); // Nd       SHARADA DIGIT TWO
  m.insert(digitsMapElement( 0x116C2, 2 )); // Nd       TAKRI DIGIT TWO
  m.insert(digitsMapElement( 0x12400, 2 )); // Nl       CUNEIFORM NUMERIC SIGN TWO ASH
  m.insert(digitsMapElement( 0x12416, 2 )); // Nl       CUNEIFORM NUMERIC SIGN TWO GESH2
  m.insert(digitsMapElement( 0x1241F, 2 )); // Nl       CUNEIFORM NUMERIC SIGN TWO GESHU
  m.insert(digitsMapElement( 0x12423, 2 )); // Nl       CUNEIFORM NUMERIC SIGN TWO SHAR2
  m.insert(digitsMapElement( 0x1242D, 2 )); // Nl       CUNEIFORM NUMERIC SIGN TWO SHARU
  m.insert(digitsMapElement( 0x12435, 2 )); // Nl       CUNEIFORM NUMERIC SIGN TWO BURU
  m.insert(digitsMapElement( 0x1244A, 2 )); // Nl       CUNEIFORM NUMERIC SIGN TWO ASH TENU
  m.insert(digitsMapElement( 0x12450, 2 )); // Nl       CUNEIFORM NUMERIC SIGN TWO BAN2
  m.insert(digitsMapElement( 0x12456, 2 )); // Nl       CUNEIFORM NUMERIC SIGN NIGIDAMIN
  m.insert(digitsMapElement( 0x12459, 2 )); // Nl       CUNEIFORM NUMERIC SIGN TWO ESHE3
  m.insert(digitsMapElement( 0x1D361, 2 )); // No       COUNTING ROD UNIT DIGIT TWO
  m.insert(digitsMapElement( 0x1D7D0, 2 )); // Nd       MATHEMATICAL BOLD DIGIT TWO
  m.insert(digitsMapElement( 0x1D7DA, 2 )); // Nd       MATHEMATICAL DOUBLE-STRUCK DIGIT TWO
  m.insert(digitsMapElement( 0x1D7E4, 2 )); // Nd       MATHEMATICAL SANS-SERIF DIGIT TWO
  m.insert(digitsMapElement( 0x1D7EE, 2 )); // Nd       MATHEMATICAL SANS-SERIF BOLD DIGIT TWO
  m.insert(digitsMapElement( 0x1D7F8, 2 )); // Nd       MATHEMATICAL MONOSPACE DIGIT TWO
  m.insert(digitsMapElement( 0x1F103, 2 )); // No       DIGIT TWO COMMA
  m.insert(digitsMapElement( 0x22390, 2 )); // Lo       CJK UNIFIED IDEOGRAPH-22390

  m.insert(digitsMapElement( 0x0F2C , 5.0/2.0 )); // No       TIBETAN DIGIT HALF THREE

  m.insert(digitsMapElement( 0x0033 , 3 )); // Nd       DIGIT THREE
  m.insert(digitsMapElement( 0x00B3 , 3 )); // No       SUPERSCRIPT THREE
  m.insert(digitsMapElement( 0x0663 , 3 )); // Nd       ARABIC-INDIC DIGIT THREE
  m.insert(digitsMapElement( 0x06F3 , 3 )); // Nd       EXTENDED ARABIC-INDIC DIGIT THREE
  m.insert(digitsMapElement( 0x07C3 , 3 )); // Nd       NKO DIGIT THREE
  m.insert(digitsMapElement( 0x0969 , 3 )); // Nd       DEVANAGARI DIGIT THREE
  m.insert(digitsMapElement( 0x09E9 , 3 )); // Nd       BENGALI DIGIT THREE
  m.insert(digitsMapElement( 0x0A69 , 3 )); // Nd       GURMUKHI DIGIT THREE
  m.insert(digitsMapElement( 0x0AE9 , 3 )); // Nd       GUJARATI DIGIT THREE
  m.insert(digitsMapElement( 0x0B69 , 3 )); // Nd       ORIYA DIGIT THREE
  m.insert(digitsMapElement( 0x0BE9 , 3 )); // Nd       TAMIL DIGIT THREE
  m.insert(digitsMapElement( 0x0C69 , 3 )); // Nd       TELUGU DIGIT THREE
  m.insert(digitsMapElement( 0x0C7B , 3 )); // No       TELUGU FRACTION DIGIT THREE FOR ODD POWERS OF FOUR
  m.insert(digitsMapElement( 0x0C7E , 3 )); // No       TELUGU FRACTION DIGIT THREE FOR EVEN POWERS OF FOUR
  m.insert(digitsMapElement( 0x0CE9 , 3 )); // Nd       KANNADA DIGIT THREE
  m.insert(digitsMapElement( 0x0D69 , 3 )); // Nd       MALAYALAM DIGIT THREE
  m.insert(digitsMapElement( 0x0E53 , 3 )); // Nd       THAI DIGIT THREE
  m.insert(digitsMapElement( 0x0ED3 , 3 )); // Nd       LAO DIGIT THREE
  m.insert(digitsMapElement( 0x0F23 , 3 )); // Nd       TIBETAN DIGIT THREE
  m.insert(digitsMapElement( 0x1043 , 3 )); // Nd       MYANMAR DIGIT THREE
  m.insert(digitsMapElement( 0x1093 , 3 )); // Nd       MYANMAR SHAN DIGIT THREE
  m.insert(digitsMapElement( 0x136B , 3 )); // No       ETHIOPIC DIGIT THREE
  m.insert(digitsMapElement( 0x17E3 , 3 )); // Nd       KHMER DIGIT THREE
  m.insert(digitsMapElement( 0x17F3 , 3 )); // No       KHMER SYMBOL LEK ATTAK BEI
  m.insert(digitsMapElement( 0x1813 , 3 )); // Nd       MONGOLIAN DIGIT THREE
  m.insert(digitsMapElement( 0x1949 , 3 )); // Nd       LIMBU DIGIT THREE
  m.insert(digitsMapElement( 0x19D3 , 3 )); // Nd       NEW TAI LUE DIGIT THREE
  m.insert(digitsMapElement( 0x1A83 , 3 )); // Nd       TAI THAM HORA DIGIT THREE
  m.insert(digitsMapElement( 0x1A93 , 3 )); // Nd       TAI THAM THAM DIGIT THREE
  m.insert(digitsMapElement( 0x1B53 , 3 )); // Nd       BALINESE DIGIT THREE
  m.insert(digitsMapElement( 0x1BB3 , 3 )); // Nd       SUNDANESE DIGIT THREE
  m.insert(digitsMapElement( 0x1C43 , 3 )); // Nd       LEPCHA DIGIT THREE
  m.insert(digitsMapElement( 0x1C53 , 3 )); // Nd       OL CHIKI DIGIT THREE
  m.insert(digitsMapElement( 0x2083 , 3 )); // No       SUBSCRIPT THREE
  m.insert(digitsMapElement( 0x2162 , 3 )); // Nl       ROMAN NUMERAL THREE
  m.insert(digitsMapElement( 0x2172 , 3 )); // Nl       SMALL ROMAN NUMERAL THREE
  m.insert(digitsMapElement( 0x2462 , 3 )); // No       CIRCLED DIGIT THREE
  m.insert(digitsMapElement( 0x2476 , 3 )); // No       PARENTHESIZED DIGIT THREE
  m.insert(digitsMapElement( 0x248A , 3 )); // No       DIGIT THREE FULL STOP
  m.insert(digitsMapElement( 0x24F7 , 3 )); // No       DOUBLE CIRCLED DIGIT THREE
  m.insert(digitsMapElement( 0x2778 , 3 )); // No       DINGBAT NEGATIVE CIRCLED DIGIT THREE
  m.insert(digitsMapElement( 0x2782 , 3 )); // No       DINGBAT CIRCLED SANS-SERIF DIGIT THREE
  m.insert(digitsMapElement( 0x278C , 3 )); // No       DINGBAT NEGATIVE CIRCLED SANS-SERIF DIGIT THREE
  m.insert(digitsMapElement( 0x3023 , 3 )); // Nl       HANGZHOU NUMERAL THREE
  m.insert(digitsMapElement( 0x3194 , 3 )); // No       IDEOGRAPHIC ANNOTATION THREE MARK
  m.insert(digitsMapElement( 0x3222 , 3 )); // No       PARENTHESIZED IDEOGRAPH THREE
  m.insert(digitsMapElement( 0x3282 , 3 )); // No       CIRCLED IDEOGRAPH THREE
  m.insert(digitsMapElement( 0x4E09 , 3 )); // Lo       CJK UNIFIED IDEOGRAPH-4E09
  m.insert(digitsMapElement( 0x4EE8 , 3 )); // Lo       CJK UNIFIED IDEOGRAPH-4EE8
  m.insert(digitsMapElement( 0x53C1 , 3 )); // Lo   [4] CJK UNIFIED IDEOGRAPH-53C1..CJK UNIFIED IDEOGRAPH-53C4
  m.insert(digitsMapElement( 0x53C2 , 3 )); // Lo   [4] CJK UNIFIED IDEOGRAPH-53C1..CJK UNIFIED IDEOGRAPH-53C4
  m.insert(digitsMapElement( 0x53C3 , 3 )); // Lo   [4] CJK UNIFIED IDEOGRAPH-53C1..CJK UNIFIED IDEOGRAPH-53C4
  m.insert(digitsMapElement( 0x53C4 , 3 )); // Lo   [4] CJK UNIFIED IDEOGRAPH-53C1..CJK UNIFIED IDEOGRAPH-53C4
  m.insert(digitsMapElement( 0x5F0E , 3 )); // Lo       CJK UNIFIED IDEOGRAPH-5F0E
  m.insert(digitsMapElement( 0xA623 , 3 )); // Nd       VAI DIGIT THREE
  m.insert(digitsMapElement( 0xA6E8 , 3 )); // Nl       BAMUM LETTER TET
  m.insert(digitsMapElement( 0xA8D3 , 3 )); // Nd       SAURASHTRA DIGIT THREE
  m.insert(digitsMapElement( 0xA903 , 3 )); // Nd       KAYAH LI DIGIT THREE
  m.insert(digitsMapElement( 0xA9D3 , 3 )); // Nd       JAVANESE DIGIT THREE
  m.insert(digitsMapElement( 0xAA53 , 3 )); // Nd       CHAM DIGIT THREE
  m.insert(digitsMapElement( 0xABF3 , 3 )); // Nd       MEETEI MAYEK DIGIT THREE
  m.insert(digitsMapElement( 0xF96B , 3 )); // Lo       CJK COMPATIBILITY IDEOGRAPH-F96B
  m.insert(digitsMapElement( 0xFF13 , 3 )); // Nd       FULLWIDTH DIGIT THREE
  m.insert(digitsMapElement( 0x10109, 3 )); // No       AEGEAN NUMBER THREE
  m.insert(digitsMapElement( 0x104A3, 3 )); // Nd       OSMANYA DIGIT THREE
  m.insert(digitsMapElement( 0x1085A, 3 )); // No       IMPERIAL ARAMAIC NUMBER THREE
  m.insert(digitsMapElement( 0x1091B, 3 )); // No       PHOENICIAN NUMBER THREE
  m.insert(digitsMapElement( 0x10A42, 3 )); // No       KHAROSHTHI DIGIT THREE
  m.insert(digitsMapElement( 0x10B5A, 3 )); // No       INSCRIPTIONAL PARTHIAN NUMBER THREE
  m.insert(digitsMapElement( 0x10B7A, 3 )); // No       INSCRIPTIONAL PAHLAVI NUMBER THREE
  m.insert(digitsMapElement( 0x10E62, 3 )); // No       RUMI DIGIT THREE
  m.insert(digitsMapElement( 0x11054, 3 )); // No       BRAHMI NUMBER THREE
  m.insert(digitsMapElement( 0x11069, 3 )); // Nd       BRAHMI DIGIT THREE
  m.insert(digitsMapElement( 0x110F3, 3 )); // Nd       SORA SOMPENG DIGIT THREE
  m.insert(digitsMapElement( 0x11139, 3 )); // Nd       CHAKMA DIGIT THREE
  m.insert(digitsMapElement( 0x111D3, 3 )); // Nd       SHARADA DIGIT THREE
  m.insert(digitsMapElement( 0x116C3, 3 )); // Nd       TAKRI DIGIT THREE
  m.insert(digitsMapElement( 0x12401, 3 )); // Nl       CUNEIFORM NUMERIC SIGN THREE ASH
  m.insert(digitsMapElement( 0x12408, 3 )); // Nl       CUNEIFORM NUMERIC SIGN THREE DISH
  m.insert(digitsMapElement( 0x12417, 3 )); // Nl       CUNEIFORM NUMERIC SIGN THREE GESH2
  m.insert(digitsMapElement( 0x12420, 3 )); // Nl       CUNEIFORM NUMERIC SIGN THREE GESHU
  m.insert(digitsMapElement( 0x12424, 3 )); // Nl   [2] CUNEIFORM NUMERIC SIGN THREE SHAR2..CUNEIFORM NUMERIC SIGN THREE SHAR2 VARIANT FORM
  m.insert(digitsMapElement( 0x12425, 3 )); // Nl   [2] CUNEIFORM NUMERIC SIGN THREE SHAR2..CUNEIFORM NUMERIC SIGN THREE SHAR2 VARIANT FORM
  m.insert(digitsMapElement( 0x1242E, 3 )); // Nl   [2] CUNEIFORM NUMERIC SIGN THREE SHARU..CUNEIFORM NUMERIC SIGN THREE SHARU VARIANT FORM
  m.insert(digitsMapElement( 0x1242F, 3 )); // Nl   [2] CUNEIFORM NUMERIC SIGN THREE SHARU..CUNEIFORM NUMERIC SIGN THREE SHARU VARIANT FORM
  m.insert(digitsMapElement( 0x12436, 3 )); // Nl   [2] CUNEIFORM NUMERIC SIGN THREE BURU..CUNEIFORM NUMERIC SIGN THREE BURU VARIANT FORM
  m.insert(digitsMapElement( 0x12437, 3 )); // Nl   [2] CUNEIFORM NUMERIC SIGN THREE BURU..CUNEIFORM NUMERIC SIGN THREE BURU VARIANT FORM
  m.insert(digitsMapElement( 0x1243A, 3 )); // Nl   [2] CUNEIFORM NUMERIC SIGN THREE VARIANT FORM ESH16..CUNEIFORM NUMERIC SIGN THREE VARIANT FORM ESH21
  m.insert(digitsMapElement( 0x1243B, 3 )); // Nl   [2] CUNEIFORM NUMERIC SIGN THREE VARIANT FORM ESH16..CUNEIFORM NUMERIC SIGN THREE VARIANT FORM ESH21
  m.insert(digitsMapElement( 0x1244B, 3 )); // Nl       CUNEIFORM NUMERIC SIGN THREE ASH TENU
  m.insert(digitsMapElement( 0x12451, 3 )); // Nl       CUNEIFORM NUMERIC SIGN THREE BAN2
  m.insert(digitsMapElement( 0x12457, 3 )); // Nl       CUNEIFORM NUMERIC SIGN NIGIDAESH
  m.insert(digitsMapElement( 0x1D362, 3 )); // No       COUNTING ROD UNIT DIGIT THREE
  m.insert(digitsMapElement( 0x1D7D1, 3 )); // Nd       MATHEMATICAL BOLD DIGIT THREE
  m.insert(digitsMapElement( 0x1D7DB, 3 )); // Nd       MATHEMATICAL DOUBLE-STRUCK DIGIT THREE
  m.insert(digitsMapElement( 0x1D7E5, 3 )); // Nd       MATHEMATICAL SANS-SERIF DIGIT THREE
  m.insert(digitsMapElement( 0x1D7EF, 3 )); // Nd       MATHEMATICAL SANS-SERIF BOLD DIGIT THREE
  m.insert(digitsMapElement( 0x1D7F9, 3 )); // Nd       MATHEMATICAL MONOSPACE DIGIT THREE
  m.insert(digitsMapElement( 0x1F104, 3 )); // No       DIGIT THREE COMMA
  m.insert(digitsMapElement( 0x20AFD, 3 )); // Lo       CJK UNIFIED IDEOGRAPH-20AFD
  m.insert(digitsMapElement( 0x20B19, 3 )); // Lo       CJK UNIFIED IDEOGRAPH-20B19
  m.insert(digitsMapElement( 0x22998, 3 )); // Lo       CJK UNIFIED IDEOGRAPH-22998
  m.insert(digitsMapElement( 0x23B1B, 3 )); // Lo       CJK UNIFIED IDEOGRAPH-23B1B

  m.insert(digitsMapElement( 0x0F2D , 7.0/2.0 )); // No       TIBETAN DIGIT HALF FOUR

  m.insert(digitsMapElement( 0x0034 , 4 )); // Nd       DIGIT FOUR
  m.insert(digitsMapElement( 0x0664 , 4 )); // Nd       ARABIC-INDIC DIGIT FOUR
  m.insert(digitsMapElement( 0x06F4 , 4 )); // Nd       EXTENDED ARABIC-INDIC DIGIT FOUR
  m.insert(digitsMapElement( 0x07C4 , 4 )); // Nd       NKO DIGIT FOUR
  m.insert(digitsMapElement( 0x096A , 4 )); // Nd       DEVANAGARI DIGIT FOUR
  m.insert(digitsMapElement( 0x09EA , 4 )); // Nd       BENGALI DIGIT FOUR
  m.insert(digitsMapElement( 0x0A6A , 4 )); // Nd       GURMUKHI DIGIT FOUR
  m.insert(digitsMapElement( 0x0AEA , 4 )); // Nd       GUJARATI DIGIT FOUR
  m.insert(digitsMapElement( 0x0B6A , 4 )); // Nd       ORIYA DIGIT FOUR
  m.insert(digitsMapElement( 0x0BEA , 4 )); // Nd       TAMIL DIGIT FOUR
  m.insert(digitsMapElement( 0x0C6A , 4 )); // Nd       TELUGU DIGIT FOUR
  m.insert(digitsMapElement( 0x0CEA , 4 )); // Nd       KANNADA DIGIT FOUR
  m.insert(digitsMapElement( 0x0D6A , 4 )); // Nd       MALAYALAM DIGIT FOUR
  m.insert(digitsMapElement( 0x0E54 , 4 )); // Nd       THAI DIGIT FOUR
  m.insert(digitsMapElement( 0x0ED4 , 4 )); // Nd       LAO DIGIT FOUR
  m.insert(digitsMapElement( 0x0F24 , 4 )); // Nd       TIBETAN DIGIT FOUR
  m.insert(digitsMapElement( 0x1044 , 4 )); // Nd       MYANMAR DIGIT FOUR
  m.insert(digitsMapElement( 0x1094 , 4 )); // Nd       MYANMAR SHAN DIGIT FOUR
  m.insert(digitsMapElement( 0x136C , 4 )); // No       ETHIOPIC DIGIT FOUR
  m.insert(digitsMapElement( 0x17E4 , 4 )); // Nd       KHMER DIGIT FOUR
  m.insert(digitsMapElement( 0x17F4 , 4 )); // No       KHMER SYMBOL LEK ATTAK BUON
  m.insert(digitsMapElement( 0x1814 , 4 )); // Nd       MONGOLIAN DIGIT FOUR
  m.insert(digitsMapElement( 0x194A , 4 )); // Nd       LIMBU DIGIT FOUR
  m.insert(digitsMapElement( 0x19D4 , 4 )); // Nd       NEW TAI LUE DIGIT FOUR
  m.insert(digitsMapElement( 0x1A84 , 4 )); // Nd       TAI THAM HORA DIGIT FOUR
  m.insert(digitsMapElement( 0x1A94 , 4 )); // Nd       TAI THAM THAM DIGIT FOUR
  m.insert(digitsMapElement( 0x1B54 , 4 )); // Nd       BALINESE DIGIT FOUR
  m.insert(digitsMapElement( 0x1BB4 , 4 )); // Nd       SUNDANESE DIGIT FOUR
  m.insert(digitsMapElement( 0x1C44 , 4 )); // Nd       LEPCHA DIGIT FOUR
  m.insert(digitsMapElement( 0x1C54 , 4 )); // Nd       OL CHIKI DIGIT FOUR
  m.insert(digitsMapElement( 0x2074 , 4 )); // No       SUPERSCRIPT FOUR
  m.insert(digitsMapElement( 0x2084 , 4 )); // No       SUBSCRIPT FOUR
  m.insert(digitsMapElement( 0x2163 , 4 )); // Nl       ROMAN NUMERAL FOUR
  m.insert(digitsMapElement( 0x2173 , 4 )); // Nl       SMALL ROMAN NUMERAL FOUR
  m.insert(digitsMapElement( 0x2463 , 4 )); // No       CIRCLED DIGIT FOUR
  m.insert(digitsMapElement( 0x2477 , 4 )); // No       PARENTHESIZED DIGIT FOUR
  m.insert(digitsMapElement( 0x248B , 4 )); // No       DIGIT FOUR FULL STOP
  m.insert(digitsMapElement( 0x24F8 , 4 )); // No       DOUBLE CIRCLED DIGIT FOUR
  m.insert(digitsMapElement( 0x2779 , 4 )); // No       DINGBAT NEGATIVE CIRCLED DIGIT FOUR
  m.insert(digitsMapElement( 0x2783 , 4 )); // No       DINGBAT CIRCLED SANS-SERIF DIGIT FOUR
  m.insert(digitsMapElement( 0x278D , 4 )); // No       DINGBAT NEGATIVE CIRCLED SANS-SERIF DIGIT FOUR
  m.insert(digitsMapElement( 0x3024 , 4 )); // Nl       HANGZHOU NUMERAL FOUR
  m.insert(digitsMapElement( 0x3195 , 4 )); // No       IDEOGRAPHIC ANNOTATION FOUR MARK
  m.insert(digitsMapElement( 0x3223 , 4 )); // No       PARENTHESIZED IDEOGRAPH FOUR
  m.insert(digitsMapElement( 0x3283 , 4 )); // No       CIRCLED IDEOGRAPH FOUR
  m.insert(digitsMapElement( 0x4E96 , 4 )); // Lo       CJK UNIFIED IDEOGRAPH-4E96
  m.insert(digitsMapElement( 0x56DB , 4 )); // Lo       CJK UNIFIED IDEOGRAPH-56DB
  m.insert(digitsMapElement( 0x8086 , 4 )); // Lo       CJK UNIFIED IDEOGRAPH-8086
  m.insert(digitsMapElement( 0xA624 , 4 )); // Nd       VAI DIGIT FOUR
  m.insert(digitsMapElement( 0xA6E9 , 4 )); // Nl       BAMUM LETTER KPA
  m.insert(digitsMapElement( 0xA8D4 , 4 )); // Nd       SAURASHTRA DIGIT FOUR
  m.insert(digitsMapElement( 0xA904 , 4 )); // Nd       KAYAH LI DIGIT FOUR
  m.insert(digitsMapElement( 0xA9D4 , 4 )); // Nd       JAVANESE DIGIT FOUR
  m.insert(digitsMapElement( 0xAA54 , 4 )); // Nd       CHAM DIGIT FOUR
  m.insert(digitsMapElement( 0xABF4 , 4 )); // Nd       MEETEI MAYEK DIGIT FOUR
  m.insert(digitsMapElement( 0xFF14 , 4 )); // Nd       FULLWIDTH DIGIT FOUR
  m.insert(digitsMapElement( 0x1010A, 4 )); // No       AEGEAN NUMBER FOUR
  m.insert(digitsMapElement( 0x104A4, 4 )); // Nd       OSMANYA DIGIT FOUR
  m.insert(digitsMapElement( 0x10A43, 4 )); // No       KHAROSHTHI DIGIT FOUR
  m.insert(digitsMapElement( 0x10B5B, 4 )); // No       INSCRIPTIONAL PARTHIAN NUMBER FOUR
  m.insert(digitsMapElement( 0x10B7B, 4 )); // No       INSCRIPTIONAL PAHLAVI NUMBER FOUR
  m.insert(digitsMapElement( 0x10E63, 4 )); // No       RUMI DIGIT FOUR
  m.insert(digitsMapElement( 0x11055, 4 )); // No       BRAHMI NUMBER FOUR
  m.insert(digitsMapElement( 0x1106A, 4 )); // Nd       BRAHMI DIGIT FOUR
  m.insert(digitsMapElement( 0x110F4, 4 )); // Nd       SORA SOMPENG DIGIT FOUR
  m.insert(digitsMapElement( 0x1113A, 4 )); // Nd       CHAKMA DIGIT FOUR
  m.insert(digitsMapElement( 0x111D4, 4 )); // Nd       SHARADA DIGIT FOUR
  m.insert(digitsMapElement( 0x116C4, 4 )); // Nd       TAKRI DIGIT FOUR
  m.insert(digitsMapElement( 0x12402, 4 )); // Nl       CUNEIFORM NUMERIC SIGN FOUR ASH
  m.insert(digitsMapElement( 0x12409, 4 )); // Nl       CUNEIFORM NUMERIC SIGN FOUR DISH
  m.insert(digitsMapElement( 0x1240F, 4 )); // Nl       CUNEIFORM NUMERIC SIGN FOUR U
  m.insert(digitsMapElement( 0x12418, 4 )); // Nl       CUNEIFORM NUMERIC SIGN FOUR GESH2
  m.insert(digitsMapElement( 0x12421, 4 )); // Nl       CUNEIFORM NUMERIC SIGN FOUR GESHU
  m.insert(digitsMapElement( 0x12426, 4 )); // Nl       CUNEIFORM NUMERIC SIGN FOUR SHAR2
  m.insert(digitsMapElement( 0x12430, 4 )); // Nl       CUNEIFORM NUMERIC SIGN FOUR SHARU
  m.insert(digitsMapElement( 0x12438, 4 )); // Nl       CUNEIFORM NUMERIC SIGN FOUR BURU
  m.insert(digitsMapElement( 0x1243C, 4 )); // Nl   [4] CUNEIFORM NUMERIC SIGN FOUR VARIANT FORM LIMMU..CUNEIFORM NUMERIC SIGN FOUR VARIANT FORM LIMMU B
  m.insert(digitsMapElement( 0x1243D, 4 )); // Nl   [4] CUNEIFORM NUMERIC SIGN FOUR VARIANT FORM LIMMU..CUNEIFORM NUMERIC SIGN FOUR VARIANT FORM LIMMU B
  m.insert(digitsMapElement( 0x1243E, 4 )); // Nl   [4] CUNEIFORM NUMERIC SIGN FOUR VARIANT FORM LIMMU..CUNEIFORM NUMERIC SIGN FOUR VARIANT FORM LIMMU B
  m.insert(digitsMapElement( 0x1243F, 4 )); // Nl   [4] CUNEIFORM NUMERIC SIGN FOUR VARIANT FORM LIMMU..CUNEIFORM NUMERIC SIGN FOUR VARIANT FORM LIMMU B
  m.insert(digitsMapElement( 0x1244C, 4 )); // Nl       CUNEIFORM NUMERIC SIGN FOUR ASH TENU
  m.insert(digitsMapElement( 0x12452, 4 )); // Nl   [2] CUNEIFORM NUMERIC SIGN FOUR BAN2..CUNEIFORM NUMERIC SIGN FOUR BAN2 VARIANT FORM
  m.insert(digitsMapElement( 0x12453, 4 )); // Nl   [2] CUNEIFORM NUMERIC SIGN FOUR BAN2..CUNEIFORM NUMERIC SIGN FOUR BAN2 VARIANT FORM
  m.insert(digitsMapElement( 0x1D363, 4 )); // No       COUNTING ROD UNIT DIGIT FOUR
  m.insert(digitsMapElement( 0x1D7D2, 4 )); // Nd       MATHEMATICAL BOLD DIGIT FOUR
  m.insert(digitsMapElement( 0x1D7DC, 4 )); // Nd       MATHEMATICAL DOUBLE-STRUCK DIGIT FOUR
  m.insert(digitsMapElement( 0x1D7E6, 4 )); // Nd       MATHEMATICAL SANS-SERIF DIGIT FOUR
  m.insert(digitsMapElement( 0x1D7F0, 4 )); // Nd       MATHEMATICAL SANS-SERIF BOLD DIGIT FOUR
  m.insert(digitsMapElement( 0x1D7FA, 4 )); // Nd       MATHEMATICAL MONOSPACE DIGIT FOUR
  m.insert(digitsMapElement( 0x1F105, 4 )); // No       DIGIT FOUR COMMA
  m.insert(digitsMapElement( 0x20064, 4 )); // Lo       CJK UNIFIED IDEOGRAPH-20064
  m.insert(digitsMapElement( 0x200E2, 4 )); // Lo       CJK UNIFIED IDEOGRAPH-200E2
  m.insert(digitsMapElement( 0x2626D, 4 )); // Lo       CJK UNIFIED IDEOGRAPH-2626D

  m.insert(digitsMapElement( 0x0F2E , 9.0/2.0 )); // No       TIBETAN DIGIT HALF FIVE

  m.insert(digitsMapElement( 0x0035 , 5 )); // Nd       DIGIT FIVE
  m.insert(digitsMapElement( 0x0665 , 5 )); // Nd       ARABIC-INDIC DIGIT FIVE
  m.insert(digitsMapElement( 0x06F5 , 5 )); // Nd       EXTENDED ARABIC-INDIC DIGIT FIVE
  m.insert(digitsMapElement( 0x07C5 , 5 )); // Nd       NKO DIGIT FIVE
  m.insert(digitsMapElement( 0x096B , 5 )); // Nd       DEVANAGARI DIGIT FIVE
  m.insert(digitsMapElement( 0x09EB , 5 )); // Nd       BENGALI DIGIT FIVE
  m.insert(digitsMapElement( 0x0A6B , 5 )); // Nd       GURMUKHI DIGIT FIVE
  m.insert(digitsMapElement( 0x0AEB , 5 )); // Nd       GUJARATI DIGIT FIVE
  m.insert(digitsMapElement( 0x0B6B , 5 )); // Nd       ORIYA DIGIT FIVE
  m.insert(digitsMapElement( 0x0BEB , 5 )); // Nd       TAMIL DIGIT FIVE
  m.insert(digitsMapElement( 0x0C6B , 5 )); // Nd       TELUGU DIGIT FIVE
  m.insert(digitsMapElement( 0x0CEB , 5 )); // Nd       KANNADA DIGIT FIVE
  m.insert(digitsMapElement( 0x0D6B , 5 )); // Nd       MALAYALAM DIGIT FIVE
  m.insert(digitsMapElement( 0x0E55 , 5 )); // Nd       THAI DIGIT FIVE
  m.insert(digitsMapElement( 0x0ED5 , 5 )); // Nd       LAO DIGIT FIVE
  m.insert(digitsMapElement( 0x0F25 , 5 )); // Nd       TIBETAN DIGIT FIVE
  m.insert(digitsMapElement( 0x1045 , 5 )); // Nd       MYANMAR DIGIT FIVE
  m.insert(digitsMapElement( 0x1095 , 5 )); // Nd       MYANMAR SHAN DIGIT FIVE
  m.insert(digitsMapElement( 0x136D , 5 )); // No       ETHIOPIC DIGIT FIVE
  m.insert(digitsMapElement( 0x17E5 , 5 )); // Nd       KHMER DIGIT FIVE
  m.insert(digitsMapElement( 0x17F5 , 5 )); // No       KHMER SYMBOL LEK ATTAK PRAM
  m.insert(digitsMapElement( 0x1815 , 5 )); // Nd       MONGOLIAN DIGIT FIVE
  m.insert(digitsMapElement( 0x194B , 5 )); // Nd       LIMBU DIGIT FIVE
  m.insert(digitsMapElement( 0x19D5 , 5 )); // Nd       NEW TAI LUE DIGIT FIVE
  m.insert(digitsMapElement( 0x1A85 , 5 )); // Nd       TAI THAM HORA DIGIT FIVE
  m.insert(digitsMapElement( 0x1A95 , 5 )); // Nd       TAI THAM THAM DIGIT FIVE
  m.insert(digitsMapElement( 0x1B55 , 5 )); // Nd       BALINESE DIGIT FIVE
  m.insert(digitsMapElement( 0x1BB5 , 5 )); // Nd       SUNDANESE DIGIT FIVE
  m.insert(digitsMapElement( 0x1C45 , 5 )); // Nd       LEPCHA DIGIT FIVE
  m.insert(digitsMapElement( 0x1C55 , 5 )); // Nd       OL CHIKI DIGIT FIVE
  m.insert(digitsMapElement( 0x2075 , 5 )); // No       SUPERSCRIPT FIVE
  m.insert(digitsMapElement( 0x2085 , 5 )); // No       SUBSCRIPT FIVE
  m.insert(digitsMapElement( 0x2164 , 5 )); // Nl       ROMAN NUMERAL FIVE
  m.insert(digitsMapElement( 0x2174 , 5 )); // Nl       SMALL ROMAN NUMERAL FIVE
  m.insert(digitsMapElement( 0x2464 , 5 )); // No       CIRCLED DIGIT FIVE
  m.insert(digitsMapElement( 0x2478 , 5 )); // No       PARENTHESIZED DIGIT FIVE
  m.insert(digitsMapElement( 0x248C , 5 )); // No       DIGIT FIVE FULL STOP
  m.insert(digitsMapElement( 0x24F9 , 5 )); // No       DOUBLE CIRCLED DIGIT FIVE
  m.insert(digitsMapElement( 0x277A , 5 )); // No       DINGBAT NEGATIVE CIRCLED DIGIT FIVE
  m.insert(digitsMapElement( 0x2784 , 5 )); // No       DINGBAT CIRCLED SANS-SERIF DIGIT FIVE
  m.insert(digitsMapElement( 0x278E , 5 )); // No       DINGBAT NEGATIVE CIRCLED SANS-SERIF DIGIT FIVE
  m.insert(digitsMapElement( 0x3025 , 5 )); // Nl       HANGZHOU NUMERAL FIVE
  m.insert(digitsMapElement( 0x3224 , 5 )); // No       PARENTHESIZED IDEOGRAPH FIVE
  m.insert(digitsMapElement( 0x3284 , 5 )); // No       CIRCLED IDEOGRAPH FIVE
  m.insert(digitsMapElement( 0x3405 , 5 )); // Lo       CJK UNIFIED IDEOGRAPH-3405
  m.insert(digitsMapElement( 0x382A , 5 )); // Lo       CJK UNIFIED IDEOGRAPH-382A
  m.insert(digitsMapElement( 0x4E94 , 5 )); // Lo       CJK UNIFIED IDEOGRAPH-4E94
  m.insert(digitsMapElement( 0x4F0D , 5 )); // Lo       CJK UNIFIED IDEOGRAPH-4F0D
  m.insert(digitsMapElement( 0xA625 , 5 )); // Nd       VAI DIGIT FIVE
  m.insert(digitsMapElement( 0xA6EA , 5 )); // Nl       BAMUM LETTER TEN
  m.insert(digitsMapElement( 0xA8D5 , 5 )); // Nd       SAURASHTRA DIGIT FIVE
  m.insert(digitsMapElement( 0xA905 , 5 )); // Nd       KAYAH LI DIGIT FIVE
  m.insert(digitsMapElement( 0xA9D5 , 5 )); // Nd       JAVANESE DIGIT FIVE
  m.insert(digitsMapElement( 0xAA55 , 5 )); // Nd       CHAM DIGIT FIVE
  m.insert(digitsMapElement( 0xABF5 , 5 )); // Nd       MEETEI MAYEK DIGIT FIVE
  m.insert(digitsMapElement( 0xFF15 , 5 )); // Nd       FULLWIDTH DIGIT FIVE
  m.insert(digitsMapElement( 0x1010B, 5 )); // No       AEGEAN NUMBER FIVE
  m.insert(digitsMapElement( 0x10143, 5 )); // Nl       GREEK ACROPHONIC ATTIC FIVE
  m.insert(digitsMapElement( 0x10148, 5 )); // Nl       GREEK ACROPHONIC ATTIC FIVE TALENTS
  m.insert(digitsMapElement( 0x1014F, 5 )); // Nl       GREEK ACROPHONIC ATTIC FIVE STATERS
  m.insert(digitsMapElement( 0x1015F, 5 )); // Nl       GREEK ACROPHONIC TROEZENIAN FIVE
  m.insert(digitsMapElement( 0x10173, 5 )); // Nl       GREEK ACROPHONIC DELPHIC FIVE MNAS
  m.insert(digitsMapElement( 0x10321, 5 )); // No       OLD ITALIC NUMERAL FIVE
  m.insert(digitsMapElement( 0x104A5, 5 )); // Nd       OSMANYA DIGIT FIVE
  m.insert(digitsMapElement( 0x10E64, 5 )); // No       RUMI DIGIT FIVE
  m.insert(digitsMapElement( 0x11056, 5 )); // No       BRAHMI NUMBER FIVE
  m.insert(digitsMapElement( 0x1106B, 5 )); // Nd       BRAHMI DIGIT FIVE
  m.insert(digitsMapElement( 0x110F5, 5 )); // Nd       SORA SOMPENG DIGIT FIVE
  m.insert(digitsMapElement( 0x1113B, 5 )); // Nd       CHAKMA DIGIT FIVE
  m.insert(digitsMapElement( 0x111D5, 5 )); // Nd       SHARADA DIGIT FIVE
  m.insert(digitsMapElement( 0x116C5, 5 )); // Nd       TAKRI DIGIT FIVE
  m.insert(digitsMapElement( 0x12403, 5 )); // Nl       CUNEIFORM NUMERIC SIGN FIVE ASH
  m.insert(digitsMapElement( 0x1240A, 5 )); // Nl       CUNEIFORM NUMERIC SIGN FIVE DISH
  m.insert(digitsMapElement( 0x12410, 5 )); // Nl       CUNEIFORM NUMERIC SIGN FIVE U
  m.insert(digitsMapElement( 0x12419, 5 )); // Nl       CUNEIFORM NUMERIC SIGN FIVE GESH2
  m.insert(digitsMapElement( 0x12422, 5 )); // Nl       CUNEIFORM NUMERIC SIGN FIVE GESHU
  m.insert(digitsMapElement( 0x12427, 5 )); // Nl       CUNEIFORM NUMERIC SIGN FIVE SHAR2
  m.insert(digitsMapElement( 0x12431, 5 )); // Nl       CUNEIFORM NUMERIC SIGN FIVE SHARU
  m.insert(digitsMapElement( 0x12439, 5 )); // Nl       CUNEIFORM NUMERIC SIGN FIVE BURU
  m.insert(digitsMapElement( 0x1244D, 5 )); // Nl       CUNEIFORM NUMERIC SIGN FIVE ASH TENU
  m.insert(digitsMapElement( 0x12454, 5 )); // Nl   [2] CUNEIFORM NUMERIC SIGN FIVE BAN2..CUNEIFORM NUMERIC SIGN FIVE BAN2 VARIANT FORM
  m.insert(digitsMapElement( 0x12455, 5 )); // Nl   [2] CUNEIFORM NUMERIC SIGN FIVE BAN2..CUNEIFORM NUMERIC SIGN FIVE BAN2 VARIANT FORM
  m.insert(digitsMapElement( 0x1D364, 5 )); // No       COUNTING ROD UNIT DIGIT FIVE
  m.insert(digitsMapElement( 0x1D7D3, 5 )); // Nd       MATHEMATICAL BOLD DIGIT FIVE
  m.insert(digitsMapElement( 0x1D7DD, 5 )); // Nd       MATHEMATICAL DOUBLE-STRUCK DIGIT FIVE
  m.insert(digitsMapElement( 0x1D7E7, 5 )); // Nd       MATHEMATICAL SANS-SERIF DIGIT FIVE
  m.insert(digitsMapElement( 0x1D7F1, 5 )); // Nd       MATHEMATICAL SANS-SERIF BOLD DIGIT FIVE
  m.insert(digitsMapElement( 0x1D7FB, 5 )); // Nd       MATHEMATICAL MONOSPACE DIGIT FIVE
  m.insert(digitsMapElement( 0x1F106, 5 )); // No       DIGIT FIVE COMMA
  m.insert(digitsMapElement( 0x20121, 5 )); // Lo       CJK UNIFIED IDEOGRAPH-20121

  m.insert(digitsMapElement( 0x0F2F , 11.0/2.0 )); // No       TIBETAN DIGIT HALF SIX

  m.insert(digitsMapElement( 0x0036 , 6 )); // Nd       DIGIT SIX
  m.insert(digitsMapElement( 0x0666 , 6 )); // Nd       ARABIC-INDIC DIGIT SIX
  m.insert(digitsMapElement( 0x06F6 , 6 )); // Nd       EXTENDED ARABIC-INDIC DIGIT SIX
  m.insert(digitsMapElement( 0x07C6 , 6 )); // Nd       NKO DIGIT SIX
  m.insert(digitsMapElement( 0x096C , 6 )); // Nd       DEVANAGARI DIGIT SIX
  m.insert(digitsMapElement( 0x09EC , 6 )); // Nd       BENGALI DIGIT SIX
  m.insert(digitsMapElement( 0x0A6C , 6 )); // Nd       GURMUKHI DIGIT SIX
  m.insert(digitsMapElement( 0x0AEC , 6 )); // Nd       GUJARATI DIGIT SIX
  m.insert(digitsMapElement( 0x0B6C , 6 )); // Nd       ORIYA DIGIT SIX
  m.insert(digitsMapElement( 0x0BEC , 6 )); // Nd       TAMIL DIGIT SIX
  m.insert(digitsMapElement( 0x0C6C , 6 )); // Nd       TELUGU DIGIT SIX
  m.insert(digitsMapElement( 0x0CEC , 6 )); // Nd       KANNADA DIGIT SIX
  m.insert(digitsMapElement( 0x0D6C , 6 )); // Nd       MALAYALAM DIGIT SIX
  m.insert(digitsMapElement( 0x0E56 , 6 )); // Nd       THAI DIGIT SIX
  m.insert(digitsMapElement( 0x0ED6 , 6 )); // Nd       LAO DIGIT SIX
  m.insert(digitsMapElement( 0x0F26 , 6 )); // Nd       TIBETAN DIGIT SIX
  m.insert(digitsMapElement( 0x1046 , 6 )); // Nd       MYANMAR DIGIT SIX
  m.insert(digitsMapElement( 0x1096 , 6 )); // Nd       MYANMAR SHAN DIGIT SIX
  m.insert(digitsMapElement( 0x136E , 6 )); // No       ETHIOPIC DIGIT SIX
  m.insert(digitsMapElement( 0x17E6 , 6 )); // Nd       KHMER DIGIT SIX
  m.insert(digitsMapElement( 0x17F6 , 6 )); // No       KHMER SYMBOL LEK ATTAK PRAM-MUOY
  m.insert(digitsMapElement( 0x1816 , 6 )); // Nd       MONGOLIAN DIGIT SIX
  m.insert(digitsMapElement( 0x194C , 6 )); // Nd       LIMBU DIGIT SIX
  m.insert(digitsMapElement( 0x19D6 , 6 )); // Nd       NEW TAI LUE DIGIT SIX
  m.insert(digitsMapElement( 0x1A86 , 6 )); // Nd       TAI THAM HORA DIGIT SIX
  m.insert(digitsMapElement( 0x1A96 , 6 )); // Nd       TAI THAM THAM DIGIT SIX
  m.insert(digitsMapElement( 0x1B56 , 6 )); // Nd       BALINESE DIGIT SIX
  m.insert(digitsMapElement( 0x1BB6 , 6 )); // Nd       SUNDANESE DIGIT SIX
  m.insert(digitsMapElement( 0x1C46 , 6 )); // Nd       LEPCHA DIGIT SIX
  m.insert(digitsMapElement( 0x1C56 , 6 )); // Nd       OL CHIKI DIGIT SIX
  m.insert(digitsMapElement( 0x2076 , 6 )); // No       SUPERSCRIPT SIX
  m.insert(digitsMapElement( 0x2086 , 6 )); // No       SUBSCRIPT SIX
  m.insert(digitsMapElement( 0x2165 , 6 )); // Nl       ROMAN NUMERAL SIX
  m.insert(digitsMapElement( 0x2175 , 6 )); // Nl       SMALL ROMAN NUMERAL SIX
  m.insert(digitsMapElement( 0x2185 , 6 )); // Nl       ROMAN NUMERAL SIX LATE FORM
  m.insert(digitsMapElement( 0x2465 , 6 )); // No       CIRCLED DIGIT SIX
  m.insert(digitsMapElement( 0x2479 , 6 )); // No       PARENTHESIZED DIGIT SIX
  m.insert(digitsMapElement( 0x248D , 6 )); // No       DIGIT SIX FULL STOP
  m.insert(digitsMapElement( 0x24FA , 6 )); // No       DOUBLE CIRCLED DIGIT SIX
  m.insert(digitsMapElement( 0x277B , 6 )); // No       DINGBAT NEGATIVE CIRCLED DIGIT SIX
  m.insert(digitsMapElement( 0x2785 , 6 )); // No       DINGBAT CIRCLED SANS-SERIF DIGIT SIX
  m.insert(digitsMapElement( 0x278F , 6 )); // No       DINGBAT NEGATIVE CIRCLED SANS-SERIF DIGIT SIX
  m.insert(digitsMapElement( 0x3026 , 6 )); // Nl       HANGZHOU NUMERAL SIX
  m.insert(digitsMapElement( 0x3225 , 6 )); // No       PARENTHESIZED IDEOGRAPH SIX
  m.insert(digitsMapElement( 0x3285 , 6 )); // No       CIRCLED IDEOGRAPH SIX
  m.insert(digitsMapElement( 0x516D , 6 )); // Lo       CJK UNIFIED IDEOGRAPH-516D
  m.insert(digitsMapElement( 0x9646 , 6 )); // Lo       CJK UNIFIED IDEOGRAPH-9646
  m.insert(digitsMapElement( 0x9678 , 6 )); // Lo       CJK UNIFIED IDEOGRAPH-9678
  m.insert(digitsMapElement( 0xA626 , 6 )); // Nd       VAI DIGIT SIX
  m.insert(digitsMapElement( 0xA6EB , 6 )); // Nl       BAMUM LETTER NTUU
  m.insert(digitsMapElement( 0xA8D6 , 6 )); // Nd       SAURASHTRA DIGIT SIX
  m.insert(digitsMapElement( 0xA906 , 6 )); // Nd       KAYAH LI DIGIT SIX
  m.insert(digitsMapElement( 0xA9D6 , 6 )); // Nd       JAVANESE DIGIT SIX
  m.insert(digitsMapElement( 0xAA56 , 6 )); // Nd       CHAM DIGIT SIX
  m.insert(digitsMapElement( 0xABF6 , 6 )); // Nd       MEETEI MAYEK DIGIT SIX
  m.insert(digitsMapElement( 0xF9D1 , 6 )); // Lo       CJK COMPATIBILITY IDEOGRAPH-F9D1
  m.insert(digitsMapElement( 0xF9D3 , 6 )); // Lo       CJK COMPATIBILITY IDEOGRAPH-F9D3
  m.insert(digitsMapElement( 0xFF16 , 6 )); // Nd       FULLWIDTH DIGIT SIX
  m.insert(digitsMapElement( 0x1010C, 6 )); // No       AEGEAN NUMBER SIX
  m.insert(digitsMapElement( 0x104A6, 6 )); // Nd       OSMANYA DIGIT SIX
  m.insert(digitsMapElement( 0x10E65, 6 )); // No       RUMI DIGIT SIX
  m.insert(digitsMapElement( 0x11057, 6 )); // No       BRAHMI NUMBER SIX
  m.insert(digitsMapElement( 0x1106C, 6 )); // Nd       BRAHMI DIGIT SIX
  m.insert(digitsMapElement( 0x110F6, 6 )); // Nd       SORA SOMPENG DIGIT SIX
  m.insert(digitsMapElement( 0x1113C, 6 )); // Nd       CHAKMA DIGIT SIX
  m.insert(digitsMapElement( 0x111D6, 6 )); // Nd       SHARADA DIGIT SIX
  m.insert(digitsMapElement( 0x116C6, 6 )); // Nd       TAKRI DIGIT SIX
  m.insert(digitsMapElement( 0x12404, 6 )); // Nl       CUNEIFORM NUMERIC SIGN SIX ASH
  m.insert(digitsMapElement( 0x1240B, 6 )); // Nl       CUNEIFORM NUMERIC SIGN SIX DISH
  m.insert(digitsMapElement( 0x12411, 6 )); // Nl       CUNEIFORM NUMERIC SIGN SIX U
  m.insert(digitsMapElement( 0x1241A, 6 )); // Nl       CUNEIFORM NUMERIC SIGN SIX GESH2
  m.insert(digitsMapElement( 0x12428, 6 )); // Nl       CUNEIFORM NUMERIC SIGN SIX SHAR2
  m.insert(digitsMapElement( 0x12440, 6 )); // Nl       CUNEIFORM NUMERIC SIGN SIX VARIANT FORM ASH9
  m.insert(digitsMapElement( 0x1244E, 6 )); // Nl       CUNEIFORM NUMERIC SIGN SIX ASH TENU
  m.insert(digitsMapElement( 0x1D365, 6 )); // No       COUNTING ROD UNIT DIGIT SIX
  m.insert(digitsMapElement( 0x1D7D4, 6 )); // Nd       MATHEMATICAL BOLD DIGIT SIX
  m.insert(digitsMapElement( 0x1D7DE, 6 )); // Nd       MATHEMATICAL DOUBLE-STRUCK DIGIT SIX
  m.insert(digitsMapElement( 0x1D7E8, 6 )); // Nd       MATHEMATICAL SANS-SERIF DIGIT SIX
  m.insert(digitsMapElement( 0x1D7F2, 6 )); // Nd       MATHEMATICAL SANS-SERIF BOLD DIGIT SIX
  m.insert(digitsMapElement( 0x1D7FC, 6 )); // Nd       MATHEMATICAL MONOSPACE DIGIT SIX
  m.insert(digitsMapElement( 0x1F107, 6 )); // No       DIGIT SIX COMMA
  m.insert(digitsMapElement( 0x20AEA, 6 )); // Lo       CJK UNIFIED IDEOGRAPH-20AEA

  m.insert(digitsMapElement( 0x0F30 , 13.0/2.0 )); // No       TIBETAN DIGIT HALF SEVEN

  m.insert(digitsMapElement( 0x0037 , 7 )); // Nd       DIGIT SEVEN
  m.insert(digitsMapElement( 0x0667 , 7 )); // Nd       ARABIC-INDIC DIGIT SEVEN
  m.insert(digitsMapElement( 0x06F7 , 7 )); // Nd       EXTENDED ARABIC-INDIC DIGIT SEVEN
  m.insert(digitsMapElement( 0x07C7 , 7 )); // Nd       NKO DIGIT SEVEN
  m.insert(digitsMapElement( 0x096D , 7 )); // Nd       DEVANAGARI DIGIT SEVEN
  m.insert(digitsMapElement( 0x09ED , 7 )); // Nd       BENGALI DIGIT SEVEN
  m.insert(digitsMapElement( 0x0A6D , 7 )); // Nd       GURMUKHI DIGIT SEVEN
  m.insert(digitsMapElement( 0x0AED , 7 )); // Nd       GUJARATI DIGIT SEVEN
  m.insert(digitsMapElement( 0x0B6D , 7 )); // Nd       ORIYA DIGIT SEVEN
  m.insert(digitsMapElement( 0x0BED , 7 )); // Nd       TAMIL DIGIT SEVEN
  m.insert(digitsMapElement( 0x0C6D , 7 )); // Nd       TELUGU DIGIT SEVEN
  m.insert(digitsMapElement( 0x0CED , 7 )); // Nd       KANNADA DIGIT SEVEN
  m.insert(digitsMapElement( 0x0D6D , 7 )); // Nd       MALAYALAM DIGIT SEVEN
  m.insert(digitsMapElement( 0x0E57 , 7 )); // Nd       THAI DIGIT SEVEN
  m.insert(digitsMapElement( 0x0ED7 , 7 )); // Nd       LAO DIGIT SEVEN
  m.insert(digitsMapElement( 0x0F27 , 7 )); // Nd       TIBETAN DIGIT SEVEN
  m.insert(digitsMapElement( 0x1047 , 7 )); // Nd       MYANMAR DIGIT SEVEN
  m.insert(digitsMapElement( 0x1097 , 7 )); // Nd       MYANMAR SHAN DIGIT SEVEN
  m.insert(digitsMapElement( 0x136F , 7 )); // No       ETHIOPIC DIGIT SEVEN
  m.insert(digitsMapElement( 0x17E7 , 7 )); // Nd       KHMER DIGIT SEVEN
  m.insert(digitsMapElement( 0x17F7 , 7 )); // No       KHMER SYMBOL LEK ATTAK PRAM-PII
  m.insert(digitsMapElement( 0x1817 , 7 )); // Nd       MONGOLIAN DIGIT SEVEN
  m.insert(digitsMapElement( 0x194D , 7 )); // Nd       LIMBU DIGIT SEVEN
  m.insert(digitsMapElement( 0x19D7 , 7 )); // Nd       NEW TAI LUE DIGIT SEVEN
  m.insert(digitsMapElement( 0x1A87 , 7 )); // Nd       TAI THAM HORA DIGIT SEVEN
  m.insert(digitsMapElement( 0x1A97 , 7 )); // Nd       TAI THAM THAM DIGIT SEVEN
  m.insert(digitsMapElement( 0x1B57 , 7 )); // Nd       BALINESE DIGIT SEVEN
  m.insert(digitsMapElement( 0x1BB7 , 7 )); // Nd       SUNDANESE DIGIT SEVEN
  m.insert(digitsMapElement( 0x1C47 , 7 )); // Nd       LEPCHA DIGIT SEVEN
  m.insert(digitsMapElement( 0x1C57 , 7 )); // Nd       OL CHIKI DIGIT SEVEN
  m.insert(digitsMapElement( 0x2077 , 7 )); // No       SUPERSCRIPT SEVEN
  m.insert(digitsMapElement( 0x2087 , 7 )); // No       SUBSCRIPT SEVEN
  m.insert(digitsMapElement( 0x2166 , 7 )); // Nl       ROMAN NUMERAL SEVEN
  m.insert(digitsMapElement( 0x2176 , 7 )); // Nl       SMALL ROMAN NUMERAL SEVEN
  m.insert(digitsMapElement( 0x2466 , 7 )); // No       CIRCLED DIGIT SEVEN
  m.insert(digitsMapElement( 0x247A , 7 )); // No       PARENTHESIZED DIGIT SEVEN
  m.insert(digitsMapElement( 0x248E , 7 )); // No       DIGIT SEVEN FULL STOP
  m.insert(digitsMapElement( 0x24FB , 7 )); // No       DOUBLE CIRCLED DIGIT SEVEN
  m.insert(digitsMapElement( 0x277C , 7 )); // No       DINGBAT NEGATIVE CIRCLED DIGIT SEVEN
  m.insert(digitsMapElement( 0x2786 , 7 )); // No       DINGBAT CIRCLED SANS-SERIF DIGIT SEVEN
  m.insert(digitsMapElement( 0x2790 , 7 )); // No       DINGBAT NEGATIVE CIRCLED SANS-SERIF DIGIT SEVEN
  m.insert(digitsMapElement( 0x3027 , 7 )); // Nl       HANGZHOU NUMERAL SEVEN
  m.insert(digitsMapElement( 0x3226 , 7 )); // No       PARENTHESIZED IDEOGRAPH SEVEN
  m.insert(digitsMapElement( 0x3286 , 7 )); // No       CIRCLED IDEOGRAPH SEVEN
  m.insert(digitsMapElement( 0x3B4D , 7 )); // Lo       CJK UNIFIED IDEOGRAPH-3B4D
  m.insert(digitsMapElement( 0x4E03 , 7 )); // Lo       CJK UNIFIED IDEOGRAPH-4E03
  m.insert(digitsMapElement( 0x67D2 , 7 )); // Lo       CJK UNIFIED IDEOGRAPH-67D2
  m.insert(digitsMapElement( 0x6F06 , 7 )); // Lo       CJK UNIFIED IDEOGRAPH-6F06
  m.insert(digitsMapElement( 0xA627 , 7 )); // Nd       VAI DIGIT SEVEN
  m.insert(digitsMapElement( 0xA6EC , 7 )); // Nl       BAMUM LETTER SAMBA
  m.insert(digitsMapElement( 0xA8D7 , 7 )); // Nd       SAURASHTRA DIGIT SEVEN
  m.insert(digitsMapElement( 0xA907 , 7 )); // Nd       KAYAH LI DIGIT SEVEN
  m.insert(digitsMapElement( 0xA9D7 , 7 )); // Nd       JAVANESE DIGIT SEVEN
  m.insert(digitsMapElement( 0xAA57 , 7 )); // Nd       CHAM DIGIT SEVEN
  m.insert(digitsMapElement( 0xABF7 , 7 )); // Nd       MEETEI MAYEK DIGIT SEVEN
  m.insert(digitsMapElement( 0xFF17 , 7 )); // Nd       FULLWIDTH DIGIT SEVEN
  m.insert(digitsMapElement( 0x1010D, 7 )); // No       AEGEAN NUMBER SEVEN
  m.insert(digitsMapElement( 0x104A7, 7 )); // Nd       OSMANYA DIGIT SEVEN
  m.insert(digitsMapElement( 0x10E66, 7 )); // No       RUMI DIGIT SEVEN
  m.insert(digitsMapElement( 0x11058, 7 )); // No       BRAHMI NUMBER SEVEN
  m.insert(digitsMapElement( 0x1106D, 7 )); // Nd       BRAHMI DIGIT SEVEN
  m.insert(digitsMapElement( 0x110F7, 7 )); // Nd       SORA SOMPENG DIGIT SEVEN
  m.insert(digitsMapElement( 0x1113D, 7 )); // Nd       CHAKMA DIGIT SEVEN
  m.insert(digitsMapElement( 0x111D7, 7 )); // Nd       SHARADA DIGIT SEVEN
  m.insert(digitsMapElement( 0x116C7, 7 )); // Nd       TAKRI DIGIT SEVEN
  m.insert(digitsMapElement( 0x12405, 7 )); // Nl       CUNEIFORM NUMERIC SIGN SEVEN ASH
  m.insert(digitsMapElement( 0x1240C, 7 )); // Nl       CUNEIFORM NUMERIC SIGN SEVEN DISH
  m.insert(digitsMapElement( 0x12412, 7 )); // Nl       CUNEIFORM NUMERIC SIGN SEVEN U
  m.insert(digitsMapElement( 0x1241B, 7 )); // Nl       CUNEIFORM NUMERIC SIGN SEVEN GESH2
  m.insert(digitsMapElement( 0x12429, 7 )); // Nl       CUNEIFORM NUMERIC SIGN SEVEN SHAR2
  m.insert(digitsMapElement( 0x12441, 7 )); // Nl   [3] CUNEIFORM NUMERIC SIGN SEVEN VARIANT FORM IMIN3..CUNEIFORM NUMERIC SIGN SEVEN VARIANT FORM IMIN B
  m.insert(digitsMapElement( 0x12442, 7 )); // Nl   [3] CUNEIFORM NUMERIC SIGN SEVEN VARIANT FORM IMIN3..CUNEIFORM NUMERIC SIGN SEVEN VARIANT FORM IMIN B
  m.insert(digitsMapElement( 0x12443, 7 )); // Nl   [3] CUNEIFORM NUMERIC SIGN SEVEN VARIANT FORM IMIN3..CUNEIFORM NUMERIC SIGN SEVEN VARIANT FORM IMIN B
  m.insert(digitsMapElement( 0x1D366, 7 )); // No       COUNTING ROD UNIT DIGIT SEVEN
  m.insert(digitsMapElement( 0x1D7D5, 7 )); // Nd       MATHEMATICAL BOLD DIGIT SEVEN
  m.insert(digitsMapElement( 0x1D7DF, 7 )); // Nd       MATHEMATICAL DOUBLE-STRUCK DIGIT SEVEN
  m.insert(digitsMapElement( 0x1D7E9, 7 )); // Nd       MATHEMATICAL SANS-SERIF DIGIT SEVEN
  m.insert(digitsMapElement( 0x1D7F3, 7 )); // Nd       MATHEMATICAL SANS-SERIF BOLD DIGIT SEVEN
  m.insert(digitsMapElement( 0x1D7FD, 7 )); // Nd       MATHEMATICAL MONOSPACE DIGIT SEVEN
  m.insert(digitsMapElement( 0x1F108, 7 )); // No       DIGIT SEVEN COMMA
  m.insert(digitsMapElement( 0x20001, 7 )); // Lo       CJK UNIFIED IDEOGRAPH-20001

  m.insert(digitsMapElement( 0x0F31 , 15.0/2.0 )); // No       TIBETAN DIGIT HALF EIGHT

  m.insert(digitsMapElement( 0x0038 , 8 )); // Nd       DIGIT EIGHT
  m.insert(digitsMapElement( 0x0668 , 8 )); // Nd       ARABIC-INDIC DIGIT EIGHT
  m.insert(digitsMapElement( 0x06F8 , 8 )); // Nd       EXTENDED ARABIC-INDIC DIGIT EIGHT
  m.insert(digitsMapElement( 0x07C8 , 8 )); // Nd       NKO DIGIT EIGHT
  m.insert(digitsMapElement( 0x096E , 8 )); // Nd       DEVANAGARI DIGIT EIGHT
  m.insert(digitsMapElement( 0x09EE , 8 )); // Nd       BENGALI DIGIT EIGHT
  m.insert(digitsMapElement( 0x0A6E , 8 )); // Nd       GURMUKHI DIGIT EIGHT
  m.insert(digitsMapElement( 0x0AEE , 8 )); // Nd       GUJARATI DIGIT EIGHT
  m.insert(digitsMapElement( 0x0B6E , 8 )); // Nd       ORIYA DIGIT EIGHT
  m.insert(digitsMapElement( 0x0BEE , 8 )); // Nd       TAMIL DIGIT EIGHT
  m.insert(digitsMapElement( 0x0C6E , 8 )); // Nd       TELUGU DIGIT EIGHT
  m.insert(digitsMapElement( 0x0CEE , 8 )); // Nd       KANNADA DIGIT EIGHT
  m.insert(digitsMapElement( 0x0D6E , 8 )); // Nd       MALAYALAM DIGIT EIGHT
  m.insert(digitsMapElement( 0x0E58 , 8 )); // Nd       THAI DIGIT EIGHT
  m.insert(digitsMapElement( 0x0ED8 , 8 )); // Nd       LAO DIGIT EIGHT
  m.insert(digitsMapElement( 0x0F28 , 8 )); // Nd       TIBETAN DIGIT EIGHT
  m.insert(digitsMapElement( 0x1048 , 8 )); // Nd       MYANMAR DIGIT EIGHT
  m.insert(digitsMapElement( 0x1098 , 8 )); // Nd       MYANMAR SHAN DIGIT EIGHT
  m.insert(digitsMapElement( 0x1370 , 8 )); // No       ETHIOPIC DIGIT EIGHT
  m.insert(digitsMapElement( 0x17E8 , 8 )); // Nd       KHMER DIGIT EIGHT
  m.insert(digitsMapElement( 0x17F8 , 8 )); // No       KHMER SYMBOL LEK ATTAK PRAM-BEI
  m.insert(digitsMapElement( 0x1818 , 8 )); // Nd       MONGOLIAN DIGIT EIGHT
  m.insert(digitsMapElement( 0x194E , 8 )); // Nd       LIMBU DIGIT EIGHT
  m.insert(digitsMapElement( 0x19D8 , 8 )); // Nd       NEW TAI LUE DIGIT EIGHT
  m.insert(digitsMapElement( 0x1A88 , 8 )); // Nd       TAI THAM HORA DIGIT EIGHT
  m.insert(digitsMapElement( 0x1A98 , 8 )); // Nd       TAI THAM THAM DIGIT EIGHT
  m.insert(digitsMapElement( 0x1B58 , 8 )); // Nd       BALINESE DIGIT EIGHT
  m.insert(digitsMapElement( 0x1BB8 , 8 )); // Nd       SUNDANESE DIGIT EIGHT
  m.insert(digitsMapElement( 0x1C48 , 8 )); // Nd       LEPCHA DIGIT EIGHT
  m.insert(digitsMapElement( 0x1C58 , 8 )); // Nd       OL CHIKI DIGIT EIGHT
  m.insert(digitsMapElement( 0x2078 , 8 )); // No       SUPERSCRIPT EIGHT
  m.insert(digitsMapElement( 0x2088 , 8 )); // No       SUBSCRIPT EIGHT
  m.insert(digitsMapElement( 0x2167 , 8 )); // Nl       ROMAN NUMERAL EIGHT
  m.insert(digitsMapElement( 0x2177 , 8 )); // Nl       SMALL ROMAN NUMERAL EIGHT
  m.insert(digitsMapElement( 0x2467 , 8 )); // No       CIRCLED DIGIT EIGHT
  m.insert(digitsMapElement( 0x247B , 8 )); // No       PARENTHESIZED DIGIT EIGHT
  m.insert(digitsMapElement( 0x248F , 8 )); // No       DIGIT EIGHT FULL STOP
  m.insert(digitsMapElement( 0x24FC , 8 )); // No       DOUBLE CIRCLED DIGIT EIGHT
  m.insert(digitsMapElement( 0x277D , 8 )); // No       DINGBAT NEGATIVE CIRCLED DIGIT EIGHT
  m.insert(digitsMapElement( 0x2787 , 8 )); // No       DINGBAT CIRCLED SANS-SERIF DIGIT EIGHT
  m.insert(digitsMapElement( 0x2791 , 8 )); // No       DINGBAT NEGATIVE CIRCLED SANS-SERIF DIGIT EIGHT
  m.insert(digitsMapElement( 0x3028 , 8 )); // Nl       HANGZHOU NUMERAL EIGHT
  m.insert(digitsMapElement( 0x3227 , 8 )); // No       PARENTHESIZED IDEOGRAPH EIGHT
  m.insert(digitsMapElement( 0x3287 , 8 )); // No       CIRCLED IDEOGRAPH EIGHT
  m.insert(digitsMapElement( 0x516B , 8 )); // Lo       CJK UNIFIED IDEOGRAPH-516B
  m.insert(digitsMapElement( 0x634C , 8 )); // Lo       CJK UNIFIED IDEOGRAPH-634C
  m.insert(digitsMapElement( 0xA628 , 8 )); // Nd       VAI DIGIT EIGHT
  m.insert(digitsMapElement( 0xA6ED , 8 )); // Nl       BAMUM LETTER FAAMAE
  m.insert(digitsMapElement( 0xA8D8 , 8 )); // Nd       SAURASHTRA DIGIT EIGHT
  m.insert(digitsMapElement( 0xA908 , 8 )); // Nd       KAYAH LI DIGIT EIGHT
  m.insert(digitsMapElement( 0xA9D8 , 8 )); // Nd       JAVANESE DIGIT EIGHT
  m.insert(digitsMapElement( 0xAA58 , 8 )); // Nd       CHAM DIGIT EIGHT
  m.insert(digitsMapElement( 0xABF8 , 8 )); // Nd       MEETEI MAYEK DIGIT EIGHT
  m.insert(digitsMapElement( 0xFF18 , 8 )); // Nd       FULLWIDTH DIGIT EIGHT
  m.insert(digitsMapElement( 0x1010E, 8 )); // No       AEGEAN NUMBER EIGHT
  m.insert(digitsMapElement( 0x104A8, 8 )); // Nd       OSMANYA DIGIT EIGHT
  m.insert(digitsMapElement( 0x10E67, 8 )); // No       RUMI DIGIT EIGHT
  m.insert(digitsMapElement( 0x11059, 8 )); // No       BRAHMI NUMBER EIGHT
  m.insert(digitsMapElement( 0x1106E, 8 )); // Nd       BRAHMI DIGIT EIGHT
  m.insert(digitsMapElement( 0x110F8, 8 )); // Nd       SORA SOMPENG DIGIT EIGHT
  m.insert(digitsMapElement( 0x1113E, 8 )); // Nd       CHAKMA DIGIT EIGHT
  m.insert(digitsMapElement( 0x111D8, 8 )); // Nd       SHARADA DIGIT EIGHT
  m.insert(digitsMapElement( 0x116C8, 8 )); // Nd       TAKRI DIGIT EIGHT
  m.insert(digitsMapElement( 0x12406, 8 )); // Nl       CUNEIFORM NUMERIC SIGN EIGHT ASH
  m.insert(digitsMapElement( 0x1240D, 8 )); // Nl       CUNEIFORM NUMERIC SIGN EIGHT DISH
  m.insert(digitsMapElement( 0x12413, 8 )); // Nl       CUNEIFORM NUMERIC SIGN EIGHT U
  m.insert(digitsMapElement( 0x1241C, 8 )); // Nl       CUNEIFORM NUMERIC SIGN EIGHT GESH2
  m.insert(digitsMapElement( 0x1242A, 8 )); // Nl       CUNEIFORM NUMERIC SIGN EIGHT SHAR2
  m.insert(digitsMapElement( 0x12444, 8 )); // Nl   [2] CUNEIFORM NUMERIC SIGN EIGHT VARIANT FORM USSU..CUNEIFORM NUMERIC SIGN EIGHT VARIANT FORM USSU3
  m.insert(digitsMapElement( 0x12445, 8 )); // Nl   [2] CUNEIFORM NUMERIC SIGN EIGHT VARIANT FORM USSU..CUNEIFORM NUMERIC SIGN EIGHT VARIANT FORM USSU3
  m.insert(digitsMapElement( 0x1D367, 8 )); // No       COUNTING ROD UNIT DIGIT EIGHT
  m.insert(digitsMapElement( 0x1D7D6, 8 )); // Nd       MATHEMATICAL BOLD DIGIT EIGHT
  m.insert(digitsMapElement( 0x1D7E0, 8 )); // Nd       MATHEMATICAL DOUBLE-STRUCK DIGIT EIGHT
  m.insert(digitsMapElement( 0x1D7EA, 8 )); // Nd       MATHEMATICAL SANS-SERIF DIGIT EIGHT
  m.insert(digitsMapElement( 0x1D7F4, 8 )); // Nd       MATHEMATICAL SANS-SERIF BOLD DIGIT EIGHT
  m.insert(digitsMapElement( 0x1D7FE, 8 )); // Nd       MATHEMATICAL MONOSPACE DIGIT EIGHT
  m.insert(digitsMapElement( 0x1F109, 8 )); // No       DIGIT EIGHT COMMA

  m.insert(digitsMapElement( 0x0F32 , 17.0/2.0 )); // No       TIBETAN DIGIT HALF NINE

  m.insert(digitsMapElement( 0x0039 , 9 )); // Nd       DIGIT NINE
  m.insert(digitsMapElement( 0x0669 , 9 )); // Nd       ARABIC-INDIC DIGIT NINE
  m.insert(digitsMapElement( 0x06F9 , 9 )); // Nd       EXTENDED ARABIC-INDIC DIGIT NINE
  m.insert(digitsMapElement( 0x07C9 , 9 )); // Nd       NKO DIGIT NINE
  m.insert(digitsMapElement( 0x096F , 9 )); // Nd       DEVANAGARI DIGIT NINE
  m.insert(digitsMapElement( 0x09EF , 9 )); // Nd       BENGALI DIGIT NINE
  m.insert(digitsMapElement( 0x0A6F , 9 )); // Nd       GURMUKHI DIGIT NINE
  m.insert(digitsMapElement( 0x0AEF , 9 )); // Nd       GUJARATI DIGIT NINE
  m.insert(digitsMapElement( 0x0B6F , 9 )); // Nd       ORIYA DIGIT NINE
  m.insert(digitsMapElement( 0x0BEF , 9 )); // Nd       TAMIL DIGIT NINE
  m.insert(digitsMapElement( 0x0C6F , 9 )); // Nd       TELUGU DIGIT NINE
  m.insert(digitsMapElement( 0x0CEF , 9 )); // Nd       KANNADA DIGIT NINE
  m.insert(digitsMapElement( 0x0D6F , 9 )); // Nd       MALAYALAM DIGIT NINE
  m.insert(digitsMapElement( 0x0E59 , 9 )); // Nd       THAI DIGIT NINE
  m.insert(digitsMapElement( 0x0ED9 , 9 )); // Nd       LAO DIGIT NINE
  m.insert(digitsMapElement( 0x0F29 , 9 )); // Nd       TIBETAN DIGIT NINE
  m.insert(digitsMapElement( 0x1049 , 9 )); // Nd       MYANMAR DIGIT NINE
  m.insert(digitsMapElement( 0x1099 , 9 )); // Nd       MYANMAR SHAN DIGIT NINE
  m.insert(digitsMapElement( 0x1371 , 9 )); // No       ETHIOPIC DIGIT NINE
  m.insert(digitsMapElement( 0x17E9 , 9 )); // Nd       KHMER DIGIT NINE
  m.insert(digitsMapElement( 0x17F9 , 9 )); // No       KHMER SYMBOL LEK ATTAK PRAM-BUON
  m.insert(digitsMapElement( 0x1819 , 9 )); // Nd       MONGOLIAN DIGIT NINE
  m.insert(digitsMapElement( 0x194F , 9 )); // Nd       LIMBU DIGIT NINE
  m.insert(digitsMapElement( 0x19D9 , 9 )); // Nd       NEW TAI LUE DIGIT NINE
  m.insert(digitsMapElement( 0x1A89 , 9 )); // Nd       TAI THAM HORA DIGIT NINE
  m.insert(digitsMapElement( 0x1A99 , 9 )); // Nd       TAI THAM THAM DIGIT NINE
  m.insert(digitsMapElement( 0x1B59 , 9 )); // Nd       BALINESE DIGIT NINE
  m.insert(digitsMapElement( 0x1BB9 , 9 )); // Nd       SUNDANESE DIGIT NINE
  m.insert(digitsMapElement( 0x1C49 , 9 )); // Nd       LEPCHA DIGIT NINE
  m.insert(digitsMapElement( 0x1C59 , 9 )); // Nd       OL CHIKI DIGIT NINE
  m.insert(digitsMapElement( 0x2079 , 9 )); // No       SUPERSCRIPT NINE
  m.insert(digitsMapElement( 0x2089 , 9 )); // No       SUBSCRIPT NINE
  m.insert(digitsMapElement( 0x2168 , 9 )); // Nl       ROMAN NUMERAL NINE
  m.insert(digitsMapElement( 0x2178 , 9 )); // Nl       SMALL ROMAN NUMERAL NINE
  m.insert(digitsMapElement( 0x2468 , 9 )); // No       CIRCLED DIGIT NINE
  m.insert(digitsMapElement( 0x247C , 9 )); // No       PARENTHESIZED DIGIT NINE
  m.insert(digitsMapElement( 0x2490 , 9 )); // No       DIGIT NINE FULL STOP
  m.insert(digitsMapElement( 0x24FD , 9 )); // No       DOUBLE CIRCLED DIGIT NINE
  m.insert(digitsMapElement( 0x277E , 9 )); // No       DINGBAT NEGATIVE CIRCLED DIGIT NINE
  m.insert(digitsMapElement( 0x2788 , 9 )); // No       DINGBAT CIRCLED SANS-SERIF DIGIT NINE
  m.insert(digitsMapElement( 0x2792 , 9 )); // No       DINGBAT NEGATIVE CIRCLED SANS-SERIF DIGIT NINE
  m.insert(digitsMapElement( 0x3029 , 9 )); // Nl       HANGZHOU NUMERAL NINE
  m.insert(digitsMapElement( 0x3228 , 9 )); // No       PARENTHESIZED IDEOGRAPH NINE
  m.insert(digitsMapElement( 0x3288 , 9 )); // No       CIRCLED IDEOGRAPH NINE
  m.insert(digitsMapElement( 0x4E5D , 9 )); // Lo       CJK UNIFIED IDEOGRAPH-4E5D
  m.insert(digitsMapElement( 0x5EFE , 9 )); // Lo       CJK UNIFIED IDEOGRAPH-5EFE
  m.insert(digitsMapElement( 0x7396 , 9 )); // Lo       CJK UNIFIED IDEOGRAPH-7396
  m.insert(digitsMapElement( 0xA629 , 9 )); // Nd       VAI DIGIT NINE
  m.insert(digitsMapElement( 0xA6EE , 9 )); // Nl       BAMUM LETTER KOVUU
  m.insert(digitsMapElement( 0xA8D9 , 9 )); // Nd       SAURASHTRA DIGIT NINE
  m.insert(digitsMapElement( 0xA909 , 9 )); // Nd       KAYAH LI DIGIT NINE
  m.insert(digitsMapElement( 0xA9D9 , 9 )); // Nd       JAVANESE DIGIT NINE
  m.insert(digitsMapElement( 0xAA59 , 9 )); // Nd       CHAM DIGIT NINE
  m.insert(digitsMapElement( 0xABF9 , 9 )); // Nd       MEETEI MAYEK DIGIT NINE
  m.insert(digitsMapElement( 0xFF19 , 9 )); // Nd       FULLWIDTH DIGIT NINE
  m.insert(digitsMapElement( 0x1010F, 9 )); // No       AEGEAN NUMBER NINE
  m.insert(digitsMapElement( 0x104A9, 9 )); // Nd       OSMANYA DIGIT NINE
  m.insert(digitsMapElement( 0x10E68, 9 )); // No       RUMI DIGIT NINE
  m.insert(digitsMapElement( 0x1105A, 9 )); // No       BRAHMI NUMBER NINE
  m.insert(digitsMapElement( 0x1106F, 9 )); // Nd       BRAHMI DIGIT NINE
  m.insert(digitsMapElement( 0x110F9, 9 )); // Nd       SORA SOMPENG DIGIT NINE
  m.insert(digitsMapElement( 0x1113F, 9 )); // Nd       CHAKMA DIGIT NINE
  m.insert(digitsMapElement( 0x111D9, 9 )); // Nd       SHARADA DIGIT NINE
  m.insert(digitsMapElement( 0x116C9, 9 )); // Nd       TAKRI DIGIT NINE
  m.insert(digitsMapElement( 0x12407, 9 )); // Nl       CUNEIFORM NUMERIC SIGN NINE ASH
  m.insert(digitsMapElement( 0x1240E, 9 )); // Nl       CUNEIFORM NUMERIC SIGN NINE DISH
  m.insert(digitsMapElement( 0x12414, 9 )); // Nl       CUNEIFORM NUMERIC SIGN NINE U
  m.insert(digitsMapElement( 0x1241D, 9 )); // Nl       CUNEIFORM NUMERIC SIGN NINE GESH2
  m.insert(digitsMapElement( 0x1242B, 9 )); // Nl       CUNEIFORM NUMERIC SIGN NINE SHAR2
  m.insert(digitsMapElement( 0x12446, 9 )); // Nl   [4] CUNEIFORM NUMERIC SIGN NINE VARIANT FORM ILIMMU..CUNEIFORM NUMERIC SIGN NINE VARIANT FORM ILIMMU A
  m.insert(digitsMapElement( 0x12447, 9 )); // Nl   [4] CUNEIFORM NUMERIC SIGN NINE VARIANT FORM ILIMMU..CUNEIFORM NUMERIC SIGN NINE VARIANT FORM ILIMMU A
  m.insert(digitsMapElement( 0x12448, 9 )); // Nl   [4] CUNEIFORM NUMERIC SIGN NINE VARIANT FORM ILIMMU..CUNEIFORM NUMERIC SIGN NINE VARIANT FORM ILIMMU A
  m.insert(digitsMapElement( 0x12449, 9 )); // Nl   [4] CUNEIFORM NUMERIC SIGN NINE VARIANT FORM ILIMMU..CUNEIFORM NUMERIC SIGN NINE VARIANT FORM ILIMMU A
  m.insert(digitsMapElement( 0x1D368, 9 )); // No       COUNTING ROD UNIT DIGIT NINE
  m.insert(digitsMapElement( 0x1D7D7, 9 )); // Nd       MATHEMATICAL BOLD DIGIT NINE
  m.insert(digitsMapElement( 0x1D7E1, 9 )); // Nd       MATHEMATICAL DOUBLE-STRUCK DIGIT NINE
  m.insert(digitsMapElement( 0x1D7EB, 9 )); // Nd       MATHEMATICAL SANS-SERIF DIGIT NINE
  m.insert(digitsMapElement( 0x1D7F5, 9 )); // Nd       MATHEMATICAL SANS-SERIF BOLD DIGIT NINE
  m.insert(digitsMapElement( 0x1D7FF, 9 )); // Nd       MATHEMATICAL MONOSPACE DIGIT NINE
  m.insert(digitsMapElement( 0x1F10A, 9 )); // No       DIGIT NINE COMMA
  m.insert(digitsMapElement( 0x2F890, 9 )); // Lo       CJK COMPATIBILITY IDEOGRAPH-2F890

  m.insert(digitsMapElement( 0x0BF0 , 10 )); // No       TAMIL NUMBER TEN
  m.insert(digitsMapElement( 0x0D70 , 10 )); // No       MALAYALAM NUMBER TEN
  m.insert(digitsMapElement( 0x1372 , 10 )); // No       ETHIOPIC NUMBER TEN
  m.insert(digitsMapElement( 0x2169 , 10 )); // Nl       ROMAN NUMERAL TEN
  m.insert(digitsMapElement( 0x2179 , 10 )); // Nl       SMALL ROMAN NUMERAL TEN
  m.insert(digitsMapElement( 0x2469 , 10 )); // No       CIRCLED NUMBER TEN
  m.insert(digitsMapElement( 0x247D , 10 )); // No       PARENTHESIZED NUMBER TEN
  m.insert(digitsMapElement( 0x2491 , 10 )); // No       NUMBER TEN FULL STOP
  m.insert(digitsMapElement( 0x24FE , 10 )); // No       DOUBLE CIRCLED NUMBER TEN
  m.insert(digitsMapElement( 0x277F , 10 )); // No       DINGBAT NEGATIVE CIRCLED NUMBER TEN
  m.insert(digitsMapElement( 0x2789 , 10 )); // No       DINGBAT CIRCLED SANS-SERIF NUMBER TEN
  m.insert(digitsMapElement( 0x2793 , 10 )); // No       DINGBAT NEGATIVE CIRCLED SANS-SERIF NUMBER TEN
  m.insert(digitsMapElement( 0x3038 , 10 )); // Nl       HANGZHOU NUMERAL TEN
  m.insert(digitsMapElement( 0x3229 , 10 )); // No       PARENTHESIZED IDEOGRAPH TEN
  m.insert(digitsMapElement( 0x3248 , 10 )); // No       CIRCLED NUMBER TEN ON BLACK SQUARE
  m.insert(digitsMapElement( 0x3289 , 10 )); // No       CIRCLED IDEOGRAPH TEN
  m.insert(digitsMapElement( 0x4EC0 , 10 )); // Lo       CJK UNIFIED IDEOGRAPH-4EC0
  m.insert(digitsMapElement( 0x5341 , 10 )); // Lo       CJK UNIFIED IDEOGRAPH-5341
  m.insert(digitsMapElement( 0x62FE , 10 )); // Lo       CJK UNIFIED IDEOGRAPH-62FE
  m.insert(digitsMapElement( 0xF973 , 10 )); // Lo       CJK COMPATIBILITY IDEOGRAPH-F973
  m.insert(digitsMapElement( 0xF9FD , 10 )); // Lo       CJK COMPATIBILITY IDEOGRAPH-F9FD
  m.insert(digitsMapElement( 0x10110, 10 )); // No       AEGEAN NUMBER TEN
  m.insert(digitsMapElement( 0x10149, 10 )); // Nl       GREEK ACROPHONIC ATTIC TEN TALENTS
  m.insert(digitsMapElement( 0x10150, 10 )); // Nl       GREEK ACROPHONIC ATTIC TEN STATERS
  m.insert(digitsMapElement( 0x10157, 10 )); // Nl       GREEK ACROPHONIC ATTIC TEN MNAS
  m.insert(digitsMapElement( 0x10160, 10 )); // Nl   [5] GREEK ACROPHONIC TROEZENIAN TEN..GREEK ACROPHONIC THESPIAN TEN
  m.insert(digitsMapElement( 0x10161, 10 )); // Nl   [5] GREEK ACROPHONIC TROEZENIAN TEN..GREEK ACROPHONIC THESPIAN TEN
  m.insert(digitsMapElement( 0x10162, 10 )); // Nl   [5] GREEK ACROPHONIC TROEZENIAN TEN..GREEK ACROPHONIC THESPIAN TEN
  m.insert(digitsMapElement( 0x10163, 10 )); // Nl   [5] GREEK ACROPHONIC TROEZENIAN TEN..GREEK ACROPHONIC THESPIAN TEN
  m.insert(digitsMapElement( 0x10164, 10 )); // Nl   [5] GREEK ACROPHONIC TROEZENIAN TEN..GREEK ACROPHONIC THESPIAN TEN
  m.insert(digitsMapElement( 0x10322, 10 )); // No       OLD ITALIC NUMERAL TEN
  m.insert(digitsMapElement( 0x103D3, 10 )); // Nl       OLD PERSIAN NUMBER TEN
  m.insert(digitsMapElement( 0x1085B, 10 )); // No       IMPERIAL ARAMAIC NUMBER TEN
  m.insert(digitsMapElement( 0x10917, 10 )); // No       PHOENICIAN NUMBER TEN
  m.insert(digitsMapElement( 0x10A44, 10 )); // No       KHAROSHTHI NUMBER TEN
  m.insert(digitsMapElement( 0x10B5C, 10 )); // No       INSCRIPTIONAL PARTHIAN NUMBER TEN
  m.insert(digitsMapElement( 0x10B7C, 10 )); // No       INSCRIPTIONAL PAHLAVI NUMBER TEN
  m.insert(digitsMapElement( 0x10E69, 10 )); // No       RUMI NUMBER TEN
  m.insert(digitsMapElement( 0x1105B, 10 )); // No       BRAHMI NUMBER TEN
  m.insert(digitsMapElement( 0x1D369, 10 )); // No       COUNTING ROD TENS DIGIT ONE

  m.insert(digitsMapElement( 0x216A , 11 )); // Nl       ROMAN NUMERAL ELEVEN
  m.insert(digitsMapElement( 0x217A , 11 )); // Nl       SMALL ROMAN NUMERAL ELEVEN
  m.insert(digitsMapElement( 0x246A , 11 )); // No       CIRCLED NUMBER ELEVEN
  m.insert(digitsMapElement( 0x247E , 11 )); // No       PARENTHESIZED NUMBER ELEVEN
  m.insert(digitsMapElement( 0x2492 , 11 )); // No       NUMBER ELEVEN FULL STOP
  m.insert(digitsMapElement( 0x24EB , 11 )); // No       NEGATIVE CIRCLED NUMBER ELEVEN

  m.insert(digitsMapElement( 0x216B , 12 )); // Nl       ROMAN NUMERAL TWELVE
  m.insert(digitsMapElement( 0x217B , 12 )); // Nl       SMALL ROMAN NUMERAL TWELVE
  m.insert(digitsMapElement( 0x246B , 12 )); // No       CIRCLED NUMBER TWELVE
  m.insert(digitsMapElement( 0x247F , 12 )); // No       PARENTHESIZED NUMBER TWELVE
  m.insert(digitsMapElement( 0x2493 , 12 )); // No       NUMBER TWELVE FULL STOP
  m.insert(digitsMapElement( 0x24EC , 12 )); // No       NEGATIVE CIRCLED NUMBER TWELVE

  m.insert(digitsMapElement( 0x246C , 13 )); // No       CIRCLED NUMBER THIRTEEN
  m.insert(digitsMapElement( 0x2480 , 13 )); // No       PARENTHESIZED NUMBER THIRTEEN
  m.insert(digitsMapElement( 0x2494 , 13 )); // No       NUMBER THIRTEEN FULL STOP
  m.insert(digitsMapElement( 0x24ED , 13 )); // No       NEGATIVE CIRCLED NUMBER THIRTEEN

  m.insert(digitsMapElement( 0x246D , 14 )); // No       CIRCLED NUMBER FOURTEEN
  m.insert(digitsMapElement( 0x2481 , 14 )); // No       PARENTHESIZED NUMBER FOURTEEN
  m.insert(digitsMapElement( 0x2495 , 14 )); // No       NUMBER FOURTEEN FULL STOP
  m.insert(digitsMapElement( 0x24EE , 14 )); // No       NEGATIVE CIRCLED NUMBER FOURTEEN

  m.insert(digitsMapElement( 0x246E , 15 )); // No       CIRCLED NUMBER FIFTEEN
  m.insert(digitsMapElement( 0x2482 , 15 )); // No       PARENTHESIZED NUMBER FIFTEEN
  m.insert(digitsMapElement( 0x2496 , 15 )); // No       NUMBER FIFTEEN FULL STOP
  m.insert(digitsMapElement( 0x24EF , 15 )); // No       NEGATIVE CIRCLED NUMBER FIFTEEN

  m.insert(digitsMapElement( 0x09F9 , 16 )); // No       BENGALI CURRENCY DENOMINATOR SIXTEEN
  m.insert(digitsMapElement( 0x246F , 16 )); // No       CIRCLED NUMBER SIXTEEN
  m.insert(digitsMapElement( 0x2483 , 16 )); // No       PARENTHESIZED NUMBER SIXTEEN
  m.insert(digitsMapElement( 0x2497 , 16 )); // No       NUMBER SIXTEEN FULL STOP
  m.insert(digitsMapElement( 0x24F0 , 16 )); // No       NEGATIVE CIRCLED NUMBER SIXTEEN

  m.insert(digitsMapElement( 0x16EE , 17 )); // Nl       RUNIC ARLAUG SYMBOL
  m.insert(digitsMapElement( 0x2470 , 17 )); // No       CIRCLED NUMBER SEVENTEEN
  m.insert(digitsMapElement( 0x2484 , 17 )); // No       PARENTHESIZED NUMBER SEVENTEEN
  m.insert(digitsMapElement( 0x2498 , 17 )); // No       NUMBER SEVENTEEN FULL STOP
  m.insert(digitsMapElement( 0x24F1 , 17 )); // No       NEGATIVE CIRCLED NUMBER SEVENTEEN

  m.insert(digitsMapElement( 0x16EF , 18 )); // Nl       RUNIC TVIMADUR SYMBOL
  m.insert(digitsMapElement( 0x2471 , 18 )); // No       CIRCLED NUMBER EIGHTEEN
  m.insert(digitsMapElement( 0x2485 , 18 )); // No       PARENTHESIZED NUMBER EIGHTEEN
  m.insert(digitsMapElement( 0x2499 , 18 )); // No       NUMBER EIGHTEEN FULL STOP
  m.insert(digitsMapElement( 0x24F2 , 18 )); // No       NEGATIVE CIRCLED NUMBER EIGHTEEN

  m.insert(digitsMapElement( 0x16F0 , 19 )); // Nl       RUNIC BELGTHOR SYMBOL
  m.insert(digitsMapElement( 0x2472 , 19 )); // No       CIRCLED NUMBER NINETEEN
  m.insert(digitsMapElement( 0x2486 , 19 )); // No       PARENTHESIZED NUMBER NINETEEN
  m.insert(digitsMapElement( 0x249A , 19 )); // No       NUMBER NINETEEN FULL STOP
  m.insert(digitsMapElement( 0x24F3 , 19 )); // No       NEGATIVE CIRCLED NUMBER NINETEEN

  m.insert(digitsMapElement( 0x1373 , 20 )); // No       ETHIOPIC NUMBER TWENTY
  m.insert(digitsMapElement( 0x2473 , 20 )); // No       CIRCLED NUMBER TWENTY
  m.insert(digitsMapElement( 0x2487 , 20 )); // No       PARENTHESIZED NUMBER TWENTY
  m.insert(digitsMapElement( 0x249B , 20 )); // No       NUMBER TWENTY FULL STOP
  m.insert(digitsMapElement( 0x24F4 , 20 )); // No       NEGATIVE CIRCLED NUMBER TWENTY
  m.insert(digitsMapElement( 0x3039 , 20 )); // Nl       HANGZHOU NUMERAL TWENTY
  m.insert(digitsMapElement( 0x3249 , 20 )); // No       CIRCLED NUMBER TWENTY ON BLACK SQUARE
  m.insert(digitsMapElement( 0x5344 , 20 )); // Lo       CJK UNIFIED IDEOGRAPH-5344
  m.insert(digitsMapElement( 0x5EFF , 20 )); // Lo       CJK UNIFIED IDEOGRAPH-5EFF
  m.insert(digitsMapElement( 0x10111, 20 )); // No       AEGEAN NUMBER TWENTY
  m.insert(digitsMapElement( 0x103D4, 20 )); // Nl       OLD PERSIAN NUMBER TWENTY
  m.insert(digitsMapElement( 0x1085C, 20 )); // No       IMPERIAL ARAMAIC NUMBER TWENTY
  m.insert(digitsMapElement( 0x10918, 20 )); // No       PHOENICIAN NUMBER TWENTY
  m.insert(digitsMapElement( 0x10A45, 20 )); // No       KHAROSHTHI NUMBER TWENTY
  m.insert(digitsMapElement( 0x10B5D, 20 )); // No       INSCRIPTIONAL PARTHIAN NUMBER TWENTY
  m.insert(digitsMapElement( 0x10B7D, 20 )); // No       INSCRIPTIONAL PAHLAVI NUMBER TWENTY
  m.insert(digitsMapElement( 0x10E6A, 20 )); // No       RUMI NUMBER TWENTY
  m.insert(digitsMapElement( 0x1105C, 20 )); // No       BRAHMI NUMBER TWENTY
  m.insert(digitsMapElement( 0x1D36A, 20 )); // No       COUNTING ROD TENS DIGIT TWO

  m.insert(digitsMapElement( 0x3251 , 21 )); // No       CIRCLED NUMBER TWENTY ONE

  m.insert(digitsMapElement( 0x3252 , 22 )); // No       CIRCLED NUMBER TWENTY TWO

  m.insert(digitsMapElement( 0x3253 , 23 )); // No       CIRCLED NUMBER TWENTY THREE

  m.insert(digitsMapElement( 0x3254 , 24 )); // No       CIRCLED NUMBER TWENTY FOUR

  m.insert(digitsMapElement( 0x3255 , 25 )); // No       CIRCLED NUMBER TWENTY FIVE

  m.insert(digitsMapElement( 0x3256 , 26 )); // No       CIRCLED NUMBER TWENTY SIX

  m.insert(digitsMapElement( 0x3257 , 27 )); // No       CIRCLED NUMBER TWENTY SEVEN

  m.insert(digitsMapElement( 0x3258 , 28 )); // No       CIRCLED NUMBER TWENTY EIGHT

  m.insert(digitsMapElement( 0x3259 , 29 )); // No       CIRCLED NUMBER TWENTY NINE

  m.insert(digitsMapElement( 0x1374 , 30 )); // No       ETHIOPIC NUMBER THIRTY
  m.insert(digitsMapElement( 0x303A , 30 )); // Nl       HANGZHOU NUMERAL THIRTY
  m.insert(digitsMapElement( 0x324A , 30 )); // No       CIRCLED NUMBER THIRTY ON BLACK SQUARE
  m.insert(digitsMapElement( 0x325A , 30 )); // No       CIRCLED NUMBER THIRTY
  m.insert(digitsMapElement( 0x5345 , 30 )); // Lo       CJK UNIFIED IDEOGRAPH-5345
  m.insert(digitsMapElement( 0x10112, 30 )); // No       AEGEAN NUMBER THIRTY
  m.insert(digitsMapElement( 0x10165, 30 )); // Nl       GREEK ACROPHONIC THESPIAN THIRTY
  m.insert(digitsMapElement( 0x10E6B, 30 )); // No       RUMI NUMBER THIRTY
  m.insert(digitsMapElement( 0x1105D, 30 )); // No       BRAHMI NUMBER THIRTY
  m.insert(digitsMapElement( 0x1D36B, 30 )); // No       COUNTING ROD TENS DIGIT THREE
  m.insert(digitsMapElement( 0x20983, 30 )); // Lo       CJK UNIFIED IDEOGRAPH-20983

  m.insert(digitsMapElement( 0x325B , 31 )); // No       CIRCLED NUMBER THIRTY ONE

  m.insert(digitsMapElement( 0x325C , 32 )); // No       CIRCLED NUMBER THIRTY TWO

  m.insert(digitsMapElement( 0x325D , 33 )); // No       CIRCLED NUMBER THIRTY THREE

  m.insert(digitsMapElement( 0x325E , 34 )); // No       CIRCLED NUMBER THIRTY FOUR

  m.insert(digitsMapElement( 0x325F , 35 )); // No       CIRCLED NUMBER THIRTY FIVE

  m.insert(digitsMapElement( 0x32B1 , 36 )); // No       CIRCLED NUMBER THIRTY SIX

  m.insert(digitsMapElement( 0x32B2 , 37 )); // No       CIRCLED NUMBER THIRTY SEVEN

  m.insert(digitsMapElement( 0x32B3 , 38 )); // No       CIRCLED NUMBER THIRTY EIGHT

  m.insert(digitsMapElement( 0x32B4 , 39 )); // No       CIRCLED NUMBER THIRTY NINE

  m.insert(digitsMapElement( 0x1375 , 40 )); // No       ETHIOPIC NUMBER FORTY
  m.insert(digitsMapElement( 0x324B , 40 )); // No       CIRCLED NUMBER FORTY ON BLACK SQUARE
  m.insert(digitsMapElement( 0x32B5 , 40 )); // No       CIRCLED NUMBER FORTY
  m.insert(digitsMapElement( 0x534C , 40 )); // Lo       CJK UNIFIED IDEOGRAPH-534C
  m.insert(digitsMapElement( 0x10113, 40 )); // No       AEGEAN NUMBER FORTY
  m.insert(digitsMapElement( 0x10E6C, 40 )); // No       RUMI NUMBER FORTY
  m.insert(digitsMapElement( 0x1105E, 40 )); // No       BRAHMI NUMBER FORTY
  m.insert(digitsMapElement( 0x1D36C, 40 )); // No       COUNTING ROD TENS DIGIT FOUR
  m.insert(digitsMapElement( 0x2098C, 40 )); // Lo       CJK UNIFIED IDEOGRAPH-2098C
  m.insert(digitsMapElement( 0x2099C, 40 )); // Lo       CJK UNIFIED IDEOGRAPH-2099C

  m.insert(digitsMapElement( 0x32B6 , 41 )); // No       CIRCLED NUMBER FORTY ONE

  m.insert(digitsMapElement( 0x32B7 , 42 )); // No       CIRCLED NUMBER FORTY TWO

  m.insert(digitsMapElement( 0x32B8 , 43 )); // No       CIRCLED NUMBER FORTY THREE

  m.insert(digitsMapElement( 0x32B9 , 44 )); // No       CIRCLED NUMBER FORTY FOUR

  m.insert(digitsMapElement( 0x32BA , 45 )); // No       CIRCLED NUMBER FORTY FIVE

  m.insert(digitsMapElement( 0x32BB , 46 )); // No       CIRCLED NUMBER FORTY SIX

  m.insert(digitsMapElement( 0x32BC , 47 )); // No       CIRCLED NUMBER FORTY SEVEN

  m.insert(digitsMapElement( 0x32BD , 48 )); // No       CIRCLED NUMBER FORTY EIGHT

  m.insert(digitsMapElement( 0x32BE , 49 )); // No       CIRCLED NUMBER FORTY NINE

  m.insert(digitsMapElement( 0x1376 , 50 )); // No       ETHIOPIC NUMBER FIFTY
  m.insert(digitsMapElement( 0x216C , 50 )); // Nl       ROMAN NUMERAL FIFTY
  m.insert(digitsMapElement( 0x217C , 50 )); // Nl       SMALL ROMAN NUMERAL FIFTY
  m.insert(digitsMapElement( 0x2186 , 50 )); // Nl       ROMAN NUMERAL FIFTY EARLY FORM
  m.insert(digitsMapElement( 0x324C , 50 )); // No       CIRCLED NUMBER FIFTY ON BLACK SQUARE
  m.insert(digitsMapElement( 0x32BF , 50 )); // No       CIRCLED NUMBER FIFTY
  m.insert(digitsMapElement( 0x10114, 50 )); // No       AEGEAN NUMBER FIFTY
  m.insert(digitsMapElement( 0x10144, 50 )); // Nl       GREEK ACROPHONIC ATTIC FIFTY
  m.insert(digitsMapElement( 0x1014A, 50 )); // Nl       GREEK ACROPHONIC ATTIC FIFTY TALENTS
  m.insert(digitsMapElement( 0x10151, 50 )); // Nl       GREEK ACROPHONIC ATTIC FIFTY STATERS
  m.insert(digitsMapElement( 0x10166, 50 )); // Nl   [4] GREEK ACROPHONIC TROEZENIAN FIFTY..GREEK ACROPHONIC THESPIAN FIFTY
  m.insert(digitsMapElement( 0x10167, 50 )); // Nl   [4] GREEK ACROPHONIC TROEZENIAN FIFTY..GREEK ACROPHONIC THESPIAN FIFTY
  m.insert(digitsMapElement( 0x10168, 50 )); // Nl   [4] GREEK ACROPHONIC TROEZENIAN FIFTY..GREEK ACROPHONIC THESPIAN FIFTY
  m.insert(digitsMapElement( 0x10169, 50 )); // Nl   [4] GREEK ACROPHONIC TROEZENIAN FIFTY..GREEK ACROPHONIC THESPIAN FIFTY
  m.insert(digitsMapElement( 0x10174, 50 )); // Nl       GREEK ACROPHONIC STRATIAN FIFTY MNAS
  m.insert(digitsMapElement( 0x10323, 50 )); // No       OLD ITALIC NUMERAL FIFTY
  m.insert(digitsMapElement( 0x10A7E, 50 )); // No       OLD SOUTH ARABIAN NUMBER FIFTY
  m.insert(digitsMapElement( 0x10E6D, 50 )); // No       RUMI NUMBER FIFTY
  m.insert(digitsMapElement( 0x1105F, 50 )); // No       BRAHMI NUMBER FIFTY
  m.insert(digitsMapElement( 0x1D36D, 50 )); // No       COUNTING ROD TENS DIGIT FIVE

  m.insert(digitsMapElement( 0x1377 , 60 )); // No       ETHIOPIC NUMBER SIXTY
  m.insert(digitsMapElement( 0x324D , 60 )); // No       CIRCLED NUMBER SIXTY ON BLACK SQUARE
  m.insert(digitsMapElement( 0x10115, 60 )); // No       AEGEAN NUMBER SIXTY
  m.insert(digitsMapElement( 0x10E6E, 60 )); // No       RUMI NUMBER SIXTY
  m.insert(digitsMapElement( 0x11060, 60 )); // No       BRAHMI NUMBER SIXTY
  m.insert(digitsMapElement( 0x1D36E, 60 )); // No       COUNTING ROD TENS DIGIT SIX

  m.insert(digitsMapElement( 0x1378 , 70 )); // No       ETHIOPIC NUMBER SEVENTY
  m.insert(digitsMapElement( 0x324E , 70 )); // No       CIRCLED NUMBER SEVENTY ON BLACK SQUARE
  m.insert(digitsMapElement( 0x10116, 70 )); // No       AEGEAN NUMBER SEVENTY
  m.insert(digitsMapElement( 0x10E6F, 70 )); // No       RUMI NUMBER SEVENTY
  m.insert(digitsMapElement( 0x11061, 70 )); // No       BRAHMI NUMBER SEVENTY
  m.insert(digitsMapElement( 0x1D36F, 70 )); // No       COUNTING ROD TENS DIGIT SEVEN

  m.insert(digitsMapElement( 0x1379 , 80 )); // No       ETHIOPIC NUMBER EIGHTY
  m.insert(digitsMapElement( 0x324F , 80 )); // No       CIRCLED NUMBER EIGHTY ON BLACK SQUARE
  m.insert(digitsMapElement( 0x10117, 80 )); // No       AEGEAN NUMBER EIGHTY
  m.insert(digitsMapElement( 0x10E70, 80 )); // No       RUMI NUMBER EIGHTY
  m.insert(digitsMapElement( 0x11062, 80 )); // No       BRAHMI NUMBER EIGHTY
  m.insert(digitsMapElement( 0x1D370, 80 )); // No       COUNTING ROD TENS DIGIT EIGHT

  m.insert(digitsMapElement( 0x137A , 90 )); // No       ETHIOPIC NUMBER NINETY
  m.insert(digitsMapElement( 0x10118, 90 )); // No       AEGEAN NUMBER NINETY
  m.insert(digitsMapElement( 0x10341, 90 )); // Nl       GOTHIC LETTER NINETY
  m.insert(digitsMapElement( 0x10E71, 90 )); // No       RUMI NUMBER NINETY
  m.insert(digitsMapElement( 0x11063, 90 )); // No       BRAHMI NUMBER NINETY
  m.insert(digitsMapElement( 0x1D371, 90 )); // No       COUNTING ROD TENS DIGIT NINE

  m.insert(digitsMapElement( 0x0BF1 , 100 )); // No       TAMIL NUMBER ONE HUNDRED
  m.insert(digitsMapElement( 0x0D71 , 100 )); // No       MALAYALAM NUMBER ONE HUNDRED
  m.insert(digitsMapElement( 0x137B , 100 )); // No       ETHIOPIC NUMBER HUNDRED
  m.insert(digitsMapElement( 0x216D , 100 )); // Nl       ROMAN NUMERAL ONE HUNDRED
  m.insert(digitsMapElement( 0x217D , 100 )); // Nl       SMALL ROMAN NUMERAL ONE HUNDRED
  m.insert(digitsMapElement( 0x4F70 , 100 )); // Lo       CJK UNIFIED IDEOGRAPH-4F70
  m.insert(digitsMapElement( 0x767E , 100 )); // Lo       CJK UNIFIED IDEOGRAPH-767E
  m.insert(digitsMapElement( 0x964C , 100 )); // Lo       CJK UNIFIED IDEOGRAPH-964C
  m.insert(digitsMapElement( 0x10119, 100 )); // No       AEGEAN NUMBER ONE HUNDRED
  m.insert(digitsMapElement( 0x1014B, 100 )); // Nl       GREEK ACROPHONIC ATTIC ONE HUNDRED TALENTS
  m.insert(digitsMapElement( 0x10152, 100 )); // Nl       GREEK ACROPHONIC ATTIC ONE HUNDRED STATERS
  m.insert(digitsMapElement( 0x1016A, 100 )); // Nl       GREEK ACROPHONIC THESPIAN ONE HUNDRED
  m.insert(digitsMapElement( 0x103D5, 100 )); // Nl       OLD PERSIAN NUMBER HUNDRED
  m.insert(digitsMapElement( 0x1085D, 100 )); // No       IMPERIAL ARAMAIC NUMBER ONE HUNDRED
  m.insert(digitsMapElement( 0x10919, 100 )); // No       PHOENICIAN NUMBER ONE HUNDRED
  m.insert(digitsMapElement( 0x10A46, 100 )); // No       KHAROSHTHI NUMBER ONE HUNDRED
  m.insert(digitsMapElement( 0x10B5E, 100 )); // No       INSCRIPTIONAL PARTHIAN NUMBER ONE HUNDRED
  m.insert(digitsMapElement( 0x10B7E, 100 )); // No       INSCRIPTIONAL PAHLAVI NUMBER ONE HUNDRED
  m.insert(digitsMapElement( 0x10E72, 100 )); // No       RUMI NUMBER ONE HUNDRED
  m.insert(digitsMapElement( 0x11064, 100 )); // No       BRAHMI NUMBER ONE HUNDRED

  m.insert(digitsMapElement( 0x1011A, 200 )); // No       AEGEAN NUMBER TWO HUNDRED
  m.insert(digitsMapElement( 0x10E73, 200 )); // No       RUMI NUMBER TWO HUNDRED

  m.insert(digitsMapElement( 0x1011B, 300 )); // No       AEGEAN NUMBER THREE HUNDRED
  m.insert(digitsMapElement( 0x1016B, 300 )); // Nl       GREEK ACROPHONIC THESPIAN THREE HUNDRED
  m.insert(digitsMapElement( 0x10E74, 300 )); // No       RUMI NUMBER THREE HUNDRED

  m.insert(digitsMapElement( 0x1011C, 400 )); // No       AEGEAN NUMBER FOUR HUNDRED
  m.insert(digitsMapElement( 0x10E75, 400 )); // No       RUMI NUMBER FOUR HUNDRED

  m.insert(digitsMapElement( 0x216E , 500 )); // Nl       ROMAN NUMERAL FIVE HUNDRED
  m.insert(digitsMapElement( 0x217E , 500 )); // Nl       SMALL ROMAN NUMERAL FIVE HUNDRED
  m.insert(digitsMapElement( 0x1011D, 500 )); // No       AEGEAN NUMBER FIVE HUNDRED
  m.insert(digitsMapElement( 0x10145, 500 )); // Nl       GREEK ACROPHONIC ATTIC FIVE HUNDRED
  m.insert(digitsMapElement( 0x1014C, 500 )); // Nl       GREEK ACROPHONIC ATTIC FIVE HUNDRED TALENTS
  m.insert(digitsMapElement( 0x10153, 500 )); // Nl       GREEK ACROPHONIC ATTIC FIVE HUNDRED STATERS
  m.insert(digitsMapElement( 0x1016C, 500 )); // Nl   [5] GREEK ACROPHONIC EPIDAUREAN FIVE HUNDRED..GREEK ACROPHONIC NAXIAN FIVE HUNDRED
  m.insert(digitsMapElement( 0x1016D, 500 )); // Nl   [5] GREEK ACROPHONIC EPIDAUREAN FIVE HUNDRED..GREEK ACROPHONIC NAXIAN FIVE HUNDRED
  m.insert(digitsMapElement( 0x1016E, 500 )); // Nl   [5] GREEK ACROPHONIC EPIDAUREAN FIVE HUNDRED..GREEK ACROPHONIC NAXIAN FIVE HUNDRED
  m.insert(digitsMapElement( 0x1016F, 500 )); // Nl   [5] GREEK ACROPHONIC EPIDAUREAN FIVE HUNDRED..GREEK ACROPHONIC NAXIAN FIVE HUNDRED
  m.insert(digitsMapElement( 0x10170, 500 )); // Nl   [5] GREEK ACROPHONIC EPIDAUREAN FIVE HUNDRED..GREEK ACROPHONIC NAXIAN FIVE HUNDRED
  m.insert(digitsMapElement( 0x10E76, 500 )); // No       RUMI NUMBER FIVE HUNDRED

  m.insert(digitsMapElement( 0x1011E, 600 )); // No       AEGEAN NUMBER SIX HUNDRED
  m.insert(digitsMapElement( 0x10E77, 600 )); // No       RUMI NUMBER SIX HUNDRED

  m.insert(digitsMapElement( 0x1011F, 700 )); // No       AEGEAN NUMBER SEVEN HUNDRED
  m.insert(digitsMapElement( 0x10E78, 700 )); // No       RUMI NUMBER SEVEN HUNDRED

  m.insert(digitsMapElement( 0x10120, 800 )); // No       AEGEAN NUMBER EIGHT HUNDRED
  m.insert(digitsMapElement( 0x10E79, 800 )); // No       RUMI NUMBER EIGHT HUNDRED

  m.insert(digitsMapElement( 0x10121, 900 )); // No       AEGEAN NUMBER NINE HUNDRED
  m.insert(digitsMapElement( 0x1034A, 900 )); // Nl       GOTHIC LETTER NINE HUNDRED
  m.insert(digitsMapElement( 0x10E7A, 900 )); // No       RUMI NUMBER NINE HUNDRED

  m.insert(digitsMapElement( 0x0BF2 , 1000 )); // No       TAMIL NUMBER ONE THOUSAND
  m.insert(digitsMapElement( 0x0D72 , 1000 )); // No       MALAYALAM NUMBER ONE THOUSAND
  m.insert(digitsMapElement( 0x216F , 1000 )); // Nl       ROMAN NUMERAL ONE THOUSAND
  m.insert(digitsMapElement( 0x217F , 1000 )); // Nl   [2] SMALL ROMAN NUMERAL ONE THOUSAND..ROMAN NUMERAL ONE THOUSAND C D
  m.insert(digitsMapElement( 0x2180 , 1000 )); // Nl   [2] SMALL ROMAN NUMERAL ONE THOUSAND..ROMAN NUMERAL ONE THOUSAND C D
  m.insert(digitsMapElement( 0x4EDF , 1000 )); // Lo       CJK UNIFIED IDEOGRAPH-4EDF
  m.insert(digitsMapElement( 0x5343 , 1000 )); // Lo       CJK UNIFIED IDEOGRAPH-5343
  m.insert(digitsMapElement( 0x9621 , 1000 )); // Lo       CJK UNIFIED IDEOGRAPH-9621
  m.insert(digitsMapElement( 0x10122, 1000 )); // No       AEGEAN NUMBER ONE THOUSAND
  m.insert(digitsMapElement( 0x1014D, 1000 )); // Nl       GREEK ACROPHONIC ATTIC ONE THOUSAND TALENTS
  m.insert(digitsMapElement( 0x10154, 1000 )); // Nl       GREEK ACROPHONIC ATTIC ONE THOUSAND STATERS
  m.insert(digitsMapElement( 0x10171, 1000 )); // Nl       GREEK ACROPHONIC THESPIAN ONE THOUSAND
  m.insert(digitsMapElement( 0x1085E, 1000 )); // No       IMPERIAL ARAMAIC NUMBER ONE THOUSAND
  m.insert(digitsMapElement( 0x10A47, 1000 )); // No       KHAROSHTHI NUMBER ONE THOUSAND
  m.insert(digitsMapElement( 0x10B5F, 1000 )); // No       INSCRIPTIONAL PARTHIAN NUMBER ONE THOUSAND
  m.insert(digitsMapElement( 0x10B7F, 1000 )); // No       INSCRIPTIONAL PAHLAVI NUMBER ONE THOUSAND
  m.insert(digitsMapElement( 0x11065, 1000 )); // No       BRAHMI NUMBER ONE THOUSAND

  m.insert(digitsMapElement( 0x10123, 2000 )); // No       AEGEAN NUMBER TWO THOUSAND

  m.insert(digitsMapElement( 0x10124, 3000 )); // No       AEGEAN NUMBER THREE THOUSAND

  m.insert(digitsMapElement( 0x10125, 4000 )); // No       AEGEAN NUMBER FOUR THOUSAND

  m.insert(digitsMapElement( 0x2181 , 5000 )); // Nl       ROMAN NUMERAL FIVE THOUSAND
  m.insert(digitsMapElement( 0x10126, 5000 )); // No       AEGEAN NUMBER FIVE THOUSAND
  m.insert(digitsMapElement( 0x10146, 5000 )); // Nl       GREEK ACROPHONIC ATTIC FIVE THOUSAND
  m.insert(digitsMapElement( 0x1014E, 5000 )); // Nl       GREEK ACROPHONIC ATTIC FIVE THOUSAND TALENTS
  m.insert(digitsMapElement( 0x10172, 5000 )); // Nl       GREEK ACROPHONIC THESPIAN FIVE THOUSAND

  m.insert(digitsMapElement( 0x10127, 6000 )); // No       AEGEAN NUMBER SIX THOUSAND

  m.insert(digitsMapElement( 0x10128, 7000 )); // No       AEGEAN NUMBER SEVEN THOUSAND

  m.insert(digitsMapElement( 0x10129, 8000 )); // No       AEGEAN NUMBER EIGHT THOUSAND

  m.insert(digitsMapElement( 0x1012A, 9000 )); // No       AEGEAN NUMBER NINE THOUSAND

  m.insert(digitsMapElement( 0x137C , 10000 )); // No       ETHIOPIC NUMBER TEN THOUSAND
  m.insert(digitsMapElement( 0x2182 , 10000 )); // Nl       ROMAN NUMERAL TEN THOUSAND
  m.insert(digitsMapElement( 0x4E07 , 10000 )); // Lo       CJK UNIFIED IDEOGRAPH-4E07
  m.insert(digitsMapElement( 0x842C , 10000 )); // Lo       CJK UNIFIED IDEOGRAPH-842C
  m.insert(digitsMapElement( 0x1012B, 10000 )); // No       AEGEAN NUMBER TEN THOUSAND
  m.insert(digitsMapElement( 0x10155, 10000 )); // Nl       GREEK ACROPHONIC ATTIC TEN THOUSAND STATERS
  m.insert(digitsMapElement( 0x1085F, 10000 )); // No       IMPERIAL ARAMAIC NUMBER TEN THOUSAND

  m.insert(digitsMapElement( 0x1012C, 20000 )); // No       AEGEAN NUMBER TWENTY THOUSAND

  m.insert(digitsMapElement( 0x1012D, 30000 )); // No       AEGEAN NUMBER THIRTY THOUSAND

  m.insert(digitsMapElement( 0x1012E, 40000 )); // No       AEGEAN NUMBER FORTY THOUSAND

  m.insert(digitsMapElement( 0x2187 , 50000 )); // Nl       ROMAN NUMERAL FIFTY THOUSAND
  m.insert(digitsMapElement( 0x1012F, 50000 )); // No       AEGEAN NUMBER FIFTY THOUSAND
  m.insert(digitsMapElement( 0x10147, 50000 )); // Nl       GREEK ACROPHONIC ATTIC FIFTY THOUSAND
  m.insert(digitsMapElement( 0x10156, 50000 )); // Nl       GREEK ACROPHONIC ATTIC FIFTY THOUSAND STATERS

  m.insert(digitsMapElement( 0x10130, 60000 )); // No       AEGEAN NUMBER SIXTY THOUSAND

  m.insert(digitsMapElement( 0x10131, 70000 )); // No       AEGEAN NUMBER SEVENTY THOUSAND

  m.insert(digitsMapElement( 0x10132, 80000 )); // No       AEGEAN NUMBER EIGHTY THOUSAND

  m.insert(digitsMapElement( 0x10133, 90000 )); // No       AEGEAN NUMBER NINETY THOUSAND

  m.insert(digitsMapElement( 0x2188 , 100000 )); // Nl       ROMAN NUMERAL ONE HUNDRED THOUSAND

  m.insert(digitsMapElement( 0x12432, 216000 )); // Nl       CUNEIFORM NUMERIC SIGN SHAR2 TIMES GAL PLUS DISH

  m.insert(digitsMapElement( 0x12433, 432000 )); // Nl       CUNEIFORM NUMERIC SIGN SHAR2 TIMES GAL PLUS MIN

  m.insert(digitsMapElement( 0x4EBF , 100000000 )); // Lo       CJK UNIFIED IDEOGRAPH-4EBF
  m.insert(digitsMapElement( 0x5104 , 100000000 )); // Lo       CJK UNIFIED IDEOGRAPH-5104

  m.insert(digitsMapElement( 0x5146 , 1000000000000.0 )); // Lo       CJK UNIFIED IDEOGRAPH-5146


  return m;
};


CUtf32Utils::charcharMap CUtf32Utils::foldSimpleCharsMapFiller(void)
{
  charcharMap m;

  m.insert(charcharMapElement(0x0041, 0x0061)); // LATIN CAPITAL LETTER A
  m.insert(charcharMapElement(0x0042, 0x0062)); // LATIN CAPITAL LETTER B
  m.insert(charcharMapElement(0x0043, 0x0063)); // LATIN CAPITAL LETTER C
  m.insert(charcharMapElement(0x0044, 0x0064)); // LATIN CAPITAL LETTER D
  m.insert(charcharMapElement(0x0045, 0x0065)); // LATIN CAPITAL LETTER E
  m.insert(charcharMapElement(0x0046, 0x0066)); // LATIN CAPITAL LETTER F
  m.insert(charcharMapElement(0x0047, 0x0067)); // LATIN CAPITAL LETTER G
  m.insert(charcharMapElement(0x0048, 0x0068)); // LATIN CAPITAL LETTER H
  m.insert(charcharMapElement(0x0049, 0x0069)); // LATIN CAPITAL LETTER I
  m.insert(charcharMapElement(0x004A, 0x006A)); // LATIN CAPITAL LETTER J
  m.insert(charcharMapElement(0x004B, 0x006B)); // LATIN CAPITAL LETTER K
  m.insert(charcharMapElement(0x004C, 0x006C)); // LATIN CAPITAL LETTER L
  m.insert(charcharMapElement(0x004D, 0x006D)); // LATIN CAPITAL LETTER M
  m.insert(charcharMapElement(0x004E, 0x006E)); // LATIN CAPITAL LETTER N
  m.insert(charcharMapElement(0x004F, 0x006F)); // LATIN CAPITAL LETTER O
  m.insert(charcharMapElement(0x0050, 0x0070)); // LATIN CAPITAL LETTER P
  m.insert(charcharMapElement(0x0051, 0x0071)); // LATIN CAPITAL LETTER Q
  m.insert(charcharMapElement(0x0052, 0x0072)); // LATIN CAPITAL LETTER R
  m.insert(charcharMapElement(0x0053, 0x0073)); // LATIN CAPITAL LETTER S
  m.insert(charcharMapElement(0x0054, 0x0074)); // LATIN CAPITAL LETTER T
  m.insert(charcharMapElement(0x0055, 0x0075)); // LATIN CAPITAL LETTER U
  m.insert(charcharMapElement(0x0056, 0x0076)); // LATIN CAPITAL LETTER V
  m.insert(charcharMapElement(0x0057, 0x0077)); // LATIN CAPITAL LETTER W
  m.insert(charcharMapElement(0x0058, 0x0078)); // LATIN CAPITAL LETTER X
  m.insert(charcharMapElement(0x0059, 0x0079)); // LATIN CAPITAL LETTER Y
  m.insert(charcharMapElement(0x005A, 0x007A)); // LATIN CAPITAL LETTER Z
  m.insert(charcharMapElement(0x00B5, 0x03BC)); // MICRO SIGN
  m.insert(charcharMapElement(0x00C0, 0x00E0)); // LATIN CAPITAL LETTER A WITH GRAVE
  m.insert(charcharMapElement(0x00C1, 0x00E1)); // LATIN CAPITAL LETTER A WITH ACUTE
  m.insert(charcharMapElement(0x00C2, 0x00E2)); // LATIN CAPITAL LETTER A WITH CIRCUMFLEX
  m.insert(charcharMapElement(0x00C3, 0x00E3)); // LATIN CAPITAL LETTER A WITH TILDE
  m.insert(charcharMapElement(0x00C4, 0x00E4)); // LATIN CAPITAL LETTER A WITH DIAERESIS
  m.insert(charcharMapElement(0x00C5, 0x00E5)); // LATIN CAPITAL LETTER A WITH RING ABOVE
  m.insert(charcharMapElement(0x00C6, 0x00E6)); // LATIN CAPITAL LETTER AE
  m.insert(charcharMapElement(0x00C7, 0x00E7)); // LATIN CAPITAL LETTER C WITH CEDILLA
  m.insert(charcharMapElement(0x00C8, 0x00E8)); // LATIN CAPITAL LETTER E WITH GRAVE
  m.insert(charcharMapElement(0x00C9, 0x00E9)); // LATIN CAPITAL LETTER E WITH ACUTE
  m.insert(charcharMapElement(0x00CA, 0x00EA)); // LATIN CAPITAL LETTER E WITH CIRCUMFLEX
  m.insert(charcharMapElement(0x00CB, 0x00EB)); // LATIN CAPITAL LETTER E WITH DIAERESIS
  m.insert(charcharMapElement(0x00CC, 0x00EC)); // LATIN CAPITAL LETTER I WITH GRAVE
  m.insert(charcharMapElement(0x00CD, 0x00ED)); // LATIN CAPITAL LETTER I WITH ACUTE
  m.insert(charcharMapElement(0x00CE, 0x00EE)); // LATIN CAPITAL LETTER I WITH CIRCUMFLEX
  m.insert(charcharMapElement(0x00CF, 0x00EF)); // LATIN CAPITAL LETTER I WITH DIAERESIS
  m.insert(charcharMapElement(0x00D0, 0x00F0)); // LATIN CAPITAL LETTER ETH
  m.insert(charcharMapElement(0x00D1, 0x00F1)); // LATIN CAPITAL LETTER N WITH TILDE
  m.insert(charcharMapElement(0x00D2, 0x00F2)); // LATIN CAPITAL LETTER O WITH GRAVE
  m.insert(charcharMapElement(0x00D3, 0x00F3)); // LATIN CAPITAL LETTER O WITH ACUTE
  m.insert(charcharMapElement(0x00D4, 0x00F4)); // LATIN CAPITAL LETTER O WITH CIRCUMFLEX
  m.insert(charcharMapElement(0x00D5, 0x00F5)); // LATIN CAPITAL LETTER O WITH TILDE
  m.insert(charcharMapElement(0x00D6, 0x00F6)); // LATIN CAPITAL LETTER O WITH DIAERESIS
  m.insert(charcharMapElement(0x00D8, 0x00F8)); // LATIN CAPITAL LETTER O WITH STROKE
  m.insert(charcharMapElement(0x00D9, 0x00F9)); // LATIN CAPITAL LETTER U WITH GRAVE
  m.insert(charcharMapElement(0x00DA, 0x00FA)); // LATIN CAPITAL LETTER U WITH ACUTE
  m.insert(charcharMapElement(0x00DB, 0x00FB)); // LATIN CAPITAL LETTER U WITH CIRCUMFLEX
  m.insert(charcharMapElement(0x00DC, 0x00FC)); // LATIN CAPITAL LETTER U WITH DIAERESIS
  m.insert(charcharMapElement(0x00DD, 0x00FD)); // LATIN CAPITAL LETTER Y WITH ACUTE
  m.insert(charcharMapElement(0x00DE, 0x00FE)); // LATIN CAPITAL LETTER THORN
  m.insert(charcharMapElement(0x0100, 0x0101)); // LATIN CAPITAL LETTER A WITH MACRON
  m.insert(charcharMapElement(0x0102, 0x0103)); // LATIN CAPITAL LETTER A WITH BREVE
  m.insert(charcharMapElement(0x0104, 0x0105)); // LATIN CAPITAL LETTER A WITH OGONEK
  m.insert(charcharMapElement(0x0106, 0x0107)); // LATIN CAPITAL LETTER C WITH ACUTE
  m.insert(charcharMapElement(0x0108, 0x0109)); // LATIN CAPITAL LETTER C WITH CIRCUMFLEX
  m.insert(charcharMapElement(0x010A, 0x010B)); // LATIN CAPITAL LETTER C WITH DOT ABOVE
  m.insert(charcharMapElement(0x010C, 0x010D)); // LATIN CAPITAL LETTER C WITH CARON
  m.insert(charcharMapElement(0x010E, 0x010F)); // LATIN CAPITAL LETTER D WITH CARON
  m.insert(charcharMapElement(0x0110, 0x0111)); // LATIN CAPITAL LETTER D WITH STROKE
  m.insert(charcharMapElement(0x0112, 0x0113)); // LATIN CAPITAL LETTER E WITH MACRON
  m.insert(charcharMapElement(0x0114, 0x0115)); // LATIN CAPITAL LETTER E WITH BREVE
  m.insert(charcharMapElement(0x0116, 0x0117)); // LATIN CAPITAL LETTER E WITH DOT ABOVE
  m.insert(charcharMapElement(0x0118, 0x0119)); // LATIN CAPITAL LETTER E WITH OGONEK
  m.insert(charcharMapElement(0x011A, 0x011B)); // LATIN CAPITAL LETTER E WITH CARON
  m.insert(charcharMapElement(0x011C, 0x011D)); // LATIN CAPITAL LETTER G WITH CIRCUMFLEX
  m.insert(charcharMapElement(0x011E, 0x011F)); // LATIN CAPITAL LETTER G WITH BREVE
  m.insert(charcharMapElement(0x0120, 0x0121)); // LATIN CAPITAL LETTER G WITH DOT ABOVE
  m.insert(charcharMapElement(0x0122, 0x0123)); // LATIN CAPITAL LETTER G WITH CEDILLA
  m.insert(charcharMapElement(0x0124, 0x0125)); // LATIN CAPITAL LETTER H WITH CIRCUMFLEX
  m.insert(charcharMapElement(0x0126, 0x0127)); // LATIN CAPITAL LETTER H WITH STROKE
  m.insert(charcharMapElement(0x0128, 0x0129)); // LATIN CAPITAL LETTER I WITH TILDE
  m.insert(charcharMapElement(0x012A, 0x012B)); // LATIN CAPITAL LETTER I WITH MACRON
  m.insert(charcharMapElement(0x012C, 0x012D)); // LATIN CAPITAL LETTER I WITH BREVE
  m.insert(charcharMapElement(0x012E, 0x012F)); // LATIN CAPITAL LETTER I WITH OGONEK
  m.insert(charcharMapElement(0x0132, 0x0133)); // LATIN CAPITAL LIGATURE IJ
  m.insert(charcharMapElement(0x0134, 0x0135)); // LATIN CAPITAL LETTER J WITH CIRCUMFLEX
  m.insert(charcharMapElement(0x0136, 0x0137)); // LATIN CAPITAL LETTER K WITH CEDILLA
  m.insert(charcharMapElement(0x0139, 0x013A)); // LATIN CAPITAL LETTER L WITH ACUTE
  m.insert(charcharMapElement(0x013B, 0x013C)); // LATIN CAPITAL LETTER L WITH CEDILLA
  m.insert(charcharMapElement(0x013D, 0x013E)); // LATIN CAPITAL LETTER L WITH CARON
  m.insert(charcharMapElement(0x013F, 0x0140)); // LATIN CAPITAL LETTER L WITH MIDDLE DOT
  m.insert(charcharMapElement(0x0141, 0x0142)); // LATIN CAPITAL LETTER L WITH STROKE
  m.insert(charcharMapElement(0x0143, 0x0144)); // LATIN CAPITAL LETTER N WITH ACUTE
  m.insert(charcharMapElement(0x0145, 0x0146)); // LATIN CAPITAL LETTER N WITH CEDILLA
  m.insert(charcharMapElement(0x0147, 0x0148)); // LATIN CAPITAL LETTER N WITH CARON
  m.insert(charcharMapElement(0x014A, 0x014B)); // LATIN CAPITAL LETTER ENG
  m.insert(charcharMapElement(0x014C, 0x014D)); // LATIN CAPITAL LETTER O WITH MACRON
  m.insert(charcharMapElement(0x014E, 0x014F)); // LATIN CAPITAL LETTER O WITH BREVE
  m.insert(charcharMapElement(0x0150, 0x0151)); // LATIN CAPITAL LETTER O WITH DOUBLE ACUTE
  m.insert(charcharMapElement(0x0152, 0x0153)); // LATIN CAPITAL LIGATURE OE
  m.insert(charcharMapElement(0x0154, 0x0155)); // LATIN CAPITAL LETTER R WITH ACUTE
  m.insert(charcharMapElement(0x0156, 0x0157)); // LATIN CAPITAL LETTER R WITH CEDILLA
  m.insert(charcharMapElement(0x0158, 0x0159)); // LATIN CAPITAL LETTER R WITH CARON
  m.insert(charcharMapElement(0x015A, 0x015B)); // LATIN CAPITAL LETTER S WITH ACUTE
  m.insert(charcharMapElement(0x015C, 0x015D)); // LATIN CAPITAL LETTER S WITH CIRCUMFLEX
  m.insert(charcharMapElement(0x015E, 0x015F)); // LATIN CAPITAL LETTER S WITH CEDILLA
  m.insert(charcharMapElement(0x0160, 0x0161)); // LATIN CAPITAL LETTER S WITH CARON
  m.insert(charcharMapElement(0x0162, 0x0163)); // LATIN CAPITAL LETTER T WITH CEDILLA
  m.insert(charcharMapElement(0x0164, 0x0165)); // LATIN CAPITAL LETTER T WITH CARON
  m.insert(charcharMapElement(0x0166, 0x0167)); // LATIN CAPITAL LETTER T WITH STROKE
  m.insert(charcharMapElement(0x0168, 0x0169)); // LATIN CAPITAL LETTER U WITH TILDE
  m.insert(charcharMapElement(0x016A, 0x016B)); // LATIN CAPITAL LETTER U WITH MACRON
  m.insert(charcharMapElement(0x016C, 0x016D)); // LATIN CAPITAL LETTER U WITH BREVE
  m.insert(charcharMapElement(0x016E, 0x016F)); // LATIN CAPITAL LETTER U WITH RING ABOVE
  m.insert(charcharMapElement(0x0170, 0x0171)); // LATIN CAPITAL LETTER U WITH DOUBLE ACUTE
  m.insert(charcharMapElement(0x0172, 0x0173)); // LATIN CAPITAL LETTER U WITH OGONEK
  m.insert(charcharMapElement(0x0174, 0x0175)); // LATIN CAPITAL LETTER W WITH CIRCUMFLEX
  m.insert(charcharMapElement(0x0176, 0x0177)); // LATIN CAPITAL LETTER Y WITH CIRCUMFLEX
  m.insert(charcharMapElement(0x0178, 0x00FF)); // LATIN CAPITAL LETTER Y WITH DIAERESIS
  m.insert(charcharMapElement(0x0179, 0x017A)); // LATIN CAPITAL LETTER Z WITH ACUTE
  m.insert(charcharMapElement(0x017B, 0x017C)); // LATIN CAPITAL LETTER Z WITH DOT ABOVE
  m.insert(charcharMapElement(0x017D, 0x017E)); // LATIN CAPITAL LETTER Z WITH CARON
  m.insert(charcharMapElement(0x017F, 0x0073)); // LATIN SMALL LETTER LONG S
  m.insert(charcharMapElement(0x0181, 0x0253)); // LATIN CAPITAL LETTER B WITH HOOK
  m.insert(charcharMapElement(0x0182, 0x0183)); // LATIN CAPITAL LETTER B WITH TOPBAR
  m.insert(charcharMapElement(0x0184, 0x0185)); // LATIN CAPITAL LETTER TONE SIX
  m.insert(charcharMapElement(0x0186, 0x0254)); // LATIN CAPITAL LETTER OPEN O
  m.insert(charcharMapElement(0x0187, 0x0188)); // LATIN CAPITAL LETTER C WITH HOOK
  m.insert(charcharMapElement(0x0189, 0x0256)); // LATIN CAPITAL LETTER AFRICAN D
  m.insert(charcharMapElement(0x018A, 0x0257)); // LATIN CAPITAL LETTER D WITH HOOK
  m.insert(charcharMapElement(0x018B, 0x018C)); // LATIN CAPITAL LETTER D WITH TOPBAR
  m.insert(charcharMapElement(0x018E, 0x01DD)); // LATIN CAPITAL LETTER REVERSED E
  m.insert(charcharMapElement(0x018F, 0x0259)); // LATIN CAPITAL LETTER SCHWA
  m.insert(charcharMapElement(0x0190, 0x025B)); // LATIN CAPITAL LETTER OPEN E
  m.insert(charcharMapElement(0x0191, 0x0192)); // LATIN CAPITAL LETTER F WITH HOOK
  m.insert(charcharMapElement(0x0193, 0x0260)); // LATIN CAPITAL LETTER G WITH HOOK
  m.insert(charcharMapElement(0x0194, 0x0263)); // LATIN CAPITAL LETTER GAMMA
  m.insert(charcharMapElement(0x0196, 0x0269)); // LATIN CAPITAL LETTER IOTA
  m.insert(charcharMapElement(0x0197, 0x0268)); // LATIN CAPITAL LETTER I WITH STROKE
  m.insert(charcharMapElement(0x0198, 0x0199)); // LATIN CAPITAL LETTER K WITH HOOK
  m.insert(charcharMapElement(0x019C, 0x026F)); // LATIN CAPITAL LETTER TURNED M
  m.insert(charcharMapElement(0x019D, 0x0272)); // LATIN CAPITAL LETTER N WITH LEFT HOOK
  m.insert(charcharMapElement(0x019F, 0x0275)); // LATIN CAPITAL LETTER O WITH MIDDLE TILDE
  m.insert(charcharMapElement(0x01A0, 0x01A1)); // LATIN CAPITAL LETTER O WITH HORN
  m.insert(charcharMapElement(0x01A2, 0x01A3)); // LATIN CAPITAL LETTER OI
  m.insert(charcharMapElement(0x01A4, 0x01A5)); // LATIN CAPITAL LETTER P WITH HOOK
  m.insert(charcharMapElement(0x01A6, 0x0280)); // LATIN LETTER YR
  m.insert(charcharMapElement(0x01A7, 0x01A8)); // LATIN CAPITAL LETTER TONE TWO
  m.insert(charcharMapElement(0x01A9, 0x0283)); // LATIN CAPITAL LETTER ESH
  m.insert(charcharMapElement(0x01AC, 0x01AD)); // LATIN CAPITAL LETTER T WITH HOOK
  m.insert(charcharMapElement(0x01AE, 0x0288)); // LATIN CAPITAL LETTER T WITH RETROFLEX HOOK
  m.insert(charcharMapElement(0x01AF, 0x01B0)); // LATIN CAPITAL LETTER U WITH HORN
  m.insert(charcharMapElement(0x01B1, 0x028A)); // LATIN CAPITAL LETTER UPSILON
  m.insert(charcharMapElement(0x01B2, 0x028B)); // LATIN CAPITAL LETTER V WITH HOOK
  m.insert(charcharMapElement(0x01B3, 0x01B4)); // LATIN CAPITAL LETTER Y WITH HOOK
  m.insert(charcharMapElement(0x01B5, 0x01B6)); // LATIN CAPITAL LETTER Z WITH STROKE
  m.insert(charcharMapElement(0x01B7, 0x0292)); // LATIN CAPITAL LETTER EZH
  m.insert(charcharMapElement(0x01B8, 0x01B9)); // LATIN CAPITAL LETTER EZH REVERSED
  m.insert(charcharMapElement(0x01BC, 0x01BD)); // LATIN CAPITAL LETTER TONE FIVE
  m.insert(charcharMapElement(0x01C4, 0x01C6)); // LATIN CAPITAL LETTER DZ WITH CARON
  m.insert(charcharMapElement(0x01C5, 0x01C6)); // LATIN CAPITAL LETTER D WITH SMALL LETTER Z WITH CARON
  m.insert(charcharMapElement(0x01C7, 0x01C9)); // LATIN CAPITAL LETTER LJ
  m.insert(charcharMapElement(0x01C8, 0x01C9)); // LATIN CAPITAL LETTER L WITH SMALL LETTER J
  m.insert(charcharMapElement(0x01CA, 0x01CC)); // LATIN CAPITAL LETTER NJ
  m.insert(charcharMapElement(0x01CB, 0x01CC)); // LATIN CAPITAL LETTER N WITH SMALL LETTER J
  m.insert(charcharMapElement(0x01CD, 0x01CE)); // LATIN CAPITAL LETTER A WITH CARON
  m.insert(charcharMapElement(0x01CF, 0x01D0)); // LATIN CAPITAL LETTER I WITH CARON
  m.insert(charcharMapElement(0x01D1, 0x01D2)); // LATIN CAPITAL LETTER O WITH CARON
  m.insert(charcharMapElement(0x01D3, 0x01D4)); // LATIN CAPITAL LETTER U WITH CARON
  m.insert(charcharMapElement(0x01D5, 0x01D6)); // LATIN CAPITAL LETTER U WITH DIAERESIS AND MACRON
  m.insert(charcharMapElement(0x01D7, 0x01D8)); // LATIN CAPITAL LETTER U WITH DIAERESIS AND ACUTE
  m.insert(charcharMapElement(0x01D9, 0x01DA)); // LATIN CAPITAL LETTER U WITH DIAERESIS AND CARON
  m.insert(charcharMapElement(0x01DB, 0x01DC)); // LATIN CAPITAL LETTER U WITH DIAERESIS AND GRAVE
  m.insert(charcharMapElement(0x01DE, 0x01DF)); // LATIN CAPITAL LETTER A WITH DIAERESIS AND MACRON
  m.insert(charcharMapElement(0x01E0, 0x01E1)); // LATIN CAPITAL LETTER A WITH DOT ABOVE AND MACRON
  m.insert(charcharMapElement(0x01E2, 0x01E3)); // LATIN CAPITAL LETTER AE WITH MACRON
  m.insert(charcharMapElement(0x01E4, 0x01E5)); // LATIN CAPITAL LETTER G WITH STROKE
  m.insert(charcharMapElement(0x01E6, 0x01E7)); // LATIN CAPITAL LETTER G WITH CARON
  m.insert(charcharMapElement(0x01E8, 0x01E9)); // LATIN CAPITAL LETTER K WITH CARON
  m.insert(charcharMapElement(0x01EA, 0x01EB)); // LATIN CAPITAL LETTER O WITH OGONEK
  m.insert(charcharMapElement(0x01EC, 0x01ED)); // LATIN CAPITAL LETTER O WITH OGONEK AND MACRON
  m.insert(charcharMapElement(0x01EE, 0x01EF)); // LATIN CAPITAL LETTER EZH WITH CARON
  m.insert(charcharMapElement(0x01F1, 0x01F3)); // LATIN CAPITAL LETTER DZ
  m.insert(charcharMapElement(0x01F2, 0x01F3)); // LATIN CAPITAL LETTER D WITH SMALL LETTER Z
  m.insert(charcharMapElement(0x01F4, 0x01F5)); // LATIN CAPITAL LETTER G WITH ACUTE
  m.insert(charcharMapElement(0x01F6, 0x0195)); // LATIN CAPITAL LETTER HWAIR
  m.insert(charcharMapElement(0x01F7, 0x01BF)); // LATIN CAPITAL LETTER WYNN
  m.insert(charcharMapElement(0x01F8, 0x01F9)); // LATIN CAPITAL LETTER N WITH GRAVE
  m.insert(charcharMapElement(0x01FA, 0x01FB)); // LATIN CAPITAL LETTER A WITH RING ABOVE AND ACUTE
  m.insert(charcharMapElement(0x01FC, 0x01FD)); // LATIN CAPITAL LETTER AE WITH ACUTE
  m.insert(charcharMapElement(0x01FE, 0x01FF)); // LATIN CAPITAL LETTER O WITH STROKE AND ACUTE
  m.insert(charcharMapElement(0x0200, 0x0201)); // LATIN CAPITAL LETTER A WITH DOUBLE GRAVE
  m.insert(charcharMapElement(0x0202, 0x0203)); // LATIN CAPITAL LETTER A WITH INVERTED BREVE
  m.insert(charcharMapElement(0x0204, 0x0205)); // LATIN CAPITAL LETTER E WITH DOUBLE GRAVE
  m.insert(charcharMapElement(0x0206, 0x0207)); // LATIN CAPITAL LETTER E WITH INVERTED BREVE
  m.insert(charcharMapElement(0x0208, 0x0209)); // LATIN CAPITAL LETTER I WITH DOUBLE GRAVE
  m.insert(charcharMapElement(0x020A, 0x020B)); // LATIN CAPITAL LETTER I WITH INVERTED BREVE
  m.insert(charcharMapElement(0x020C, 0x020D)); // LATIN CAPITAL LETTER O WITH DOUBLE GRAVE
  m.insert(charcharMapElement(0x020E, 0x020F)); // LATIN CAPITAL LETTER O WITH INVERTED BREVE
  m.insert(charcharMapElement(0x0210, 0x0211)); // LATIN CAPITAL LETTER R WITH DOUBLE GRAVE
  m.insert(charcharMapElement(0x0212, 0x0213)); // LATIN CAPITAL LETTER R WITH INVERTED BREVE
  m.insert(charcharMapElement(0x0214, 0x0215)); // LATIN CAPITAL LETTER U WITH DOUBLE GRAVE
  m.insert(charcharMapElement(0x0216, 0x0217)); // LATIN CAPITAL LETTER U WITH INVERTED BREVE
  m.insert(charcharMapElement(0x0218, 0x0219)); // LATIN CAPITAL LETTER S WITH COMMA BELOW
  m.insert(charcharMapElement(0x021A, 0x021B)); // LATIN CAPITAL LETTER T WITH COMMA BELOW
  m.insert(charcharMapElement(0x021C, 0x021D)); // LATIN CAPITAL LETTER YOGH
  m.insert(charcharMapElement(0x021E, 0x021F)); // LATIN CAPITAL LETTER H WITH CARON
  m.insert(charcharMapElement(0x0220, 0x019E)); // LATIN CAPITAL LETTER N WITH LONG RIGHT LEG
  m.insert(charcharMapElement(0x0222, 0x0223)); // LATIN CAPITAL LETTER OU
  m.insert(charcharMapElement(0x0224, 0x0225)); // LATIN CAPITAL LETTER Z WITH HOOK
  m.insert(charcharMapElement(0x0226, 0x0227)); // LATIN CAPITAL LETTER A WITH DOT ABOVE
  m.insert(charcharMapElement(0x0228, 0x0229)); // LATIN CAPITAL LETTER E WITH CEDILLA
  m.insert(charcharMapElement(0x022A, 0x022B)); // LATIN CAPITAL LETTER O WITH DIAERESIS AND MACRON
  m.insert(charcharMapElement(0x022C, 0x022D)); // LATIN CAPITAL LETTER O WITH TILDE AND MACRON
  m.insert(charcharMapElement(0x022E, 0x022F)); // LATIN CAPITAL LETTER O WITH DOT ABOVE
  m.insert(charcharMapElement(0x0230, 0x0231)); // LATIN CAPITAL LETTER O WITH DOT ABOVE AND MACRON
  m.insert(charcharMapElement(0x0232, 0x0233)); // LATIN CAPITAL LETTER Y WITH MACRON
  m.insert(charcharMapElement(0x023A, 0x2C65)); // LATIN CAPITAL LETTER A WITH STROKE
  m.insert(charcharMapElement(0x023B, 0x023C)); // LATIN CAPITAL LETTER C WITH STROKE
  m.insert(charcharMapElement(0x023D, 0x019A)); // LATIN CAPITAL LETTER L WITH BAR
  m.insert(charcharMapElement(0x023E, 0x2C66)); // LATIN CAPITAL LETTER T WITH DIAGONAL STROKE
  m.insert(charcharMapElement(0x0241, 0x0242)); // LATIN CAPITAL LETTER GLOTTAL STOP
  m.insert(charcharMapElement(0x0243, 0x0180)); // LATIN CAPITAL LETTER B WITH STROKE
  m.insert(charcharMapElement(0x0244, 0x0289)); // LATIN CAPITAL LETTER U BAR
  m.insert(charcharMapElement(0x0245, 0x028C)); // LATIN CAPITAL LETTER TURNED V
  m.insert(charcharMapElement(0x0246, 0x0247)); // LATIN CAPITAL LETTER E WITH STROKE
  m.insert(charcharMapElement(0x0248, 0x0249)); // LATIN CAPITAL LETTER J WITH STROKE
  m.insert(charcharMapElement(0x024A, 0x024B)); // LATIN CAPITAL LETTER SMALL Q WITH HOOK TAIL
  m.insert(charcharMapElement(0x024C, 0x024D)); // LATIN CAPITAL LETTER R WITH STROKE
  m.insert(charcharMapElement(0x024E, 0x024F)); // LATIN CAPITAL LETTER Y WITH STROKE
  m.insert(charcharMapElement(0x0345, 0x03B9)); // COMBINING GREEK YPOGEGRAMMENI
  m.insert(charcharMapElement(0x0370, 0x0371)); // GREEK CAPITAL LETTER HETA
  m.insert(charcharMapElement(0x0372, 0x0373)); // GREEK CAPITAL LETTER ARCHAIC SAMPI
  m.insert(charcharMapElement(0x0376, 0x0377)); // GREEK CAPITAL LETTER PAMPHYLIAN DIGAMMA
  m.insert(charcharMapElement(0x0386, 0x03AC)); // GREEK CAPITAL LETTER ALPHA WITH TONOS
  m.insert(charcharMapElement(0x0388, 0x03AD)); // GREEK CAPITAL LETTER EPSILON WITH TONOS
  m.insert(charcharMapElement(0x0389, 0x03AE)); // GREEK CAPITAL LETTER ETA WITH TONOS
  m.insert(charcharMapElement(0x038A, 0x03AF)); // GREEK CAPITAL LETTER IOTA WITH TONOS
  m.insert(charcharMapElement(0x038C, 0x03CC)); // GREEK CAPITAL LETTER OMICRON WITH TONOS
  m.insert(charcharMapElement(0x038E, 0x03CD)); // GREEK CAPITAL LETTER UPSILON WITH TONOS
  m.insert(charcharMapElement(0x038F, 0x03CE)); // GREEK CAPITAL LETTER OMEGA WITH TONOS
  m.insert(charcharMapElement(0x0391, 0x03B1)); // GREEK CAPITAL LETTER ALPHA
  m.insert(charcharMapElement(0x0392, 0x03B2)); // GREEK CAPITAL LETTER BETA
  m.insert(charcharMapElement(0x0393, 0x03B3)); // GREEK CAPITAL LETTER GAMMA
  m.insert(charcharMapElement(0x0394, 0x03B4)); // GREEK CAPITAL LETTER DELTA
  m.insert(charcharMapElement(0x0395, 0x03B5)); // GREEK CAPITAL LETTER EPSILON
  m.insert(charcharMapElement(0x0396, 0x03B6)); // GREEK CAPITAL LETTER ZETA
  m.insert(charcharMapElement(0x0397, 0x03B7)); // GREEK CAPITAL LETTER ETA
  m.insert(charcharMapElement(0x0398, 0x03B8)); // GREEK CAPITAL LETTER THETA
  m.insert(charcharMapElement(0x0399, 0x03B9)); // GREEK CAPITAL LETTER IOTA
  m.insert(charcharMapElement(0x039A, 0x03BA)); // GREEK CAPITAL LETTER KAPPA
  m.insert(charcharMapElement(0x039B, 0x03BB)); // GREEK CAPITAL LETTER LAMDA
  m.insert(charcharMapElement(0x039C, 0x03BC)); // GREEK CAPITAL LETTER MU
  m.insert(charcharMapElement(0x039D, 0x03BD)); // GREEK CAPITAL LETTER NU
  m.insert(charcharMapElement(0x039E, 0x03BE)); // GREEK CAPITAL LETTER XI
  m.insert(charcharMapElement(0x039F, 0x03BF)); // GREEK CAPITAL LETTER OMICRON
  m.insert(charcharMapElement(0x03A0, 0x03C0)); // GREEK CAPITAL LETTER PI
  m.insert(charcharMapElement(0x03A1, 0x03C1)); // GREEK CAPITAL LETTER RHO
  m.insert(charcharMapElement(0x03A3, 0x03C3)); // GREEK CAPITAL LETTER SIGMA
  m.insert(charcharMapElement(0x03A4, 0x03C4)); // GREEK CAPITAL LETTER TAU
  m.insert(charcharMapElement(0x03A5, 0x03C5)); // GREEK CAPITAL LETTER UPSILON
  m.insert(charcharMapElement(0x03A6, 0x03C6)); // GREEK CAPITAL LETTER PHI
  m.insert(charcharMapElement(0x03A7, 0x03C7)); // GREEK CAPITAL LETTER CHI
  m.insert(charcharMapElement(0x03A8, 0x03C8)); // GREEK CAPITAL LETTER PSI
  m.insert(charcharMapElement(0x03A9, 0x03C9)); // GREEK CAPITAL LETTER OMEGA
  m.insert(charcharMapElement(0x03AA, 0x03CA)); // GREEK CAPITAL LETTER IOTA WITH DIALYTIKA
  m.insert(charcharMapElement(0x03AB, 0x03CB)); // GREEK CAPITAL LETTER UPSILON WITH DIALYTIKA
  m.insert(charcharMapElement(0x03C2, 0x03C3)); // GREEK SMALL LETTER FINAL SIGMA
  m.insert(charcharMapElement(0x03CF, 0x03D7)); // GREEK CAPITAL KAI SYMBOL
  m.insert(charcharMapElement(0x03D0, 0x03B2)); // GREEK BETA SYMBOL
  m.insert(charcharMapElement(0x03D1, 0x03B8)); // GREEK THETA SYMBOL
  m.insert(charcharMapElement(0x03D5, 0x03C6)); // GREEK PHI SYMBOL
  m.insert(charcharMapElement(0x03D6, 0x03C0)); // GREEK PI SYMBOL
  m.insert(charcharMapElement(0x03D8, 0x03D9)); // GREEK LETTER ARCHAIC KOPPA
  m.insert(charcharMapElement(0x03DA, 0x03DB)); // GREEK LETTER STIGMA
  m.insert(charcharMapElement(0x03DC, 0x03DD)); // GREEK LETTER DIGAMMA
  m.insert(charcharMapElement(0x03DE, 0x03DF)); // GREEK LETTER KOPPA
  m.insert(charcharMapElement(0x03E0, 0x03E1)); // GREEK LETTER SAMPI
  m.insert(charcharMapElement(0x03E2, 0x03E3)); // COPTIC CAPITAL LETTER SHEI
  m.insert(charcharMapElement(0x03E4, 0x03E5)); // COPTIC CAPITAL LETTER FEI
  m.insert(charcharMapElement(0x03E6, 0x03E7)); // COPTIC CAPITAL LETTER KHEI
  m.insert(charcharMapElement(0x03E8, 0x03E9)); // COPTIC CAPITAL LETTER HORI
  m.insert(charcharMapElement(0x03EA, 0x03EB)); // COPTIC CAPITAL LETTER GANGIA
  m.insert(charcharMapElement(0x03EC, 0x03ED)); // COPTIC CAPITAL LETTER SHIMA
  m.insert(charcharMapElement(0x03EE, 0x03EF)); // COPTIC CAPITAL LETTER DEI
  m.insert(charcharMapElement(0x03F0, 0x03BA)); // GREEK KAPPA SYMBOL
  m.insert(charcharMapElement(0x03F1, 0x03C1)); // GREEK RHO SYMBOL
  m.insert(charcharMapElement(0x03F4, 0x03B8)); // GREEK CAPITAL THETA SYMBOL
  m.insert(charcharMapElement(0x03F5, 0x03B5)); // GREEK LUNATE EPSILON SYMBOL
  m.insert(charcharMapElement(0x03F7, 0x03F8)); // GREEK CAPITAL LETTER SHO
  m.insert(charcharMapElement(0x03F9, 0x03F2)); // GREEK CAPITAL LUNATE SIGMA SYMBOL
  m.insert(charcharMapElement(0x03FA, 0x03FB)); // GREEK CAPITAL LETTER SAN
  m.insert(charcharMapElement(0x03FD, 0x037B)); // GREEK CAPITAL REVERSED LUNATE SIGMA SYMBOL
  m.insert(charcharMapElement(0x03FE, 0x037C)); // GREEK CAPITAL DOTTED LUNATE SIGMA SYMBOL
  m.insert(charcharMapElement(0x03FF, 0x037D)); // GREEK CAPITAL REVERSED DOTTED LUNATE SIGMA SYMBOL
  m.insert(charcharMapElement(0x0400, 0x0450)); // CYRILLIC CAPITAL LETTER IE WITH GRAVE
  m.insert(charcharMapElement(0x0401, 0x0451)); // CYRILLIC CAPITAL LETTER IO
  m.insert(charcharMapElement(0x0402, 0x0452)); // CYRILLIC CAPITAL LETTER DJE
  m.insert(charcharMapElement(0x0403, 0x0453)); // CYRILLIC CAPITAL LETTER GJE
  m.insert(charcharMapElement(0x0404, 0x0454)); // CYRILLIC CAPITAL LETTER UKRAINIAN IE
  m.insert(charcharMapElement(0x0405, 0x0455)); // CYRILLIC CAPITAL LETTER DZE
  m.insert(charcharMapElement(0x0406, 0x0456)); // CYRILLIC CAPITAL LETTER BYELORUSSIAN-UKRAINIAN I
  m.insert(charcharMapElement(0x0407, 0x0457)); // CYRILLIC CAPITAL LETTER YI
  m.insert(charcharMapElement(0x0408, 0x0458)); // CYRILLIC CAPITAL LETTER JE
  m.insert(charcharMapElement(0x0409, 0x0459)); // CYRILLIC CAPITAL LETTER LJE
  m.insert(charcharMapElement(0x040A, 0x045A)); // CYRILLIC CAPITAL LETTER NJE
  m.insert(charcharMapElement(0x040B, 0x045B)); // CYRILLIC CAPITAL LETTER TSHE
  m.insert(charcharMapElement(0x040C, 0x045C)); // CYRILLIC CAPITAL LETTER KJE
  m.insert(charcharMapElement(0x040D, 0x045D)); // CYRILLIC CAPITAL LETTER I WITH GRAVE
  m.insert(charcharMapElement(0x040E, 0x045E)); // CYRILLIC CAPITAL LETTER SHORT U
  m.insert(charcharMapElement(0x040F, 0x045F)); // CYRILLIC CAPITAL LETTER DZHE
  m.insert(charcharMapElement(0x0410, 0x0430)); // CYRILLIC CAPITAL LETTER A
  m.insert(charcharMapElement(0x0411, 0x0431)); // CYRILLIC CAPITAL LETTER BE
  m.insert(charcharMapElement(0x0412, 0x0432)); // CYRILLIC CAPITAL LETTER VE
  m.insert(charcharMapElement(0x0413, 0x0433)); // CYRILLIC CAPITAL LETTER GHE
  m.insert(charcharMapElement(0x0414, 0x0434)); // CYRILLIC CAPITAL LETTER DE
  m.insert(charcharMapElement(0x0415, 0x0435)); // CYRILLIC CAPITAL LETTER IE
  m.insert(charcharMapElement(0x0416, 0x0436)); // CYRILLIC CAPITAL LETTER ZHE
  m.insert(charcharMapElement(0x0417, 0x0437)); // CYRILLIC CAPITAL LETTER ZE
  m.insert(charcharMapElement(0x0418, 0x0438)); // CYRILLIC CAPITAL LETTER I
  m.insert(charcharMapElement(0x0419, 0x0439)); // CYRILLIC CAPITAL LETTER SHORT I
  m.insert(charcharMapElement(0x041A, 0x043A)); // CYRILLIC CAPITAL LETTER KA
  m.insert(charcharMapElement(0x041B, 0x043B)); // CYRILLIC CAPITAL LETTER EL
  m.insert(charcharMapElement(0x041C, 0x043C)); // CYRILLIC CAPITAL LETTER EM
  m.insert(charcharMapElement(0x041D, 0x043D)); // CYRILLIC CAPITAL LETTER EN
  m.insert(charcharMapElement(0x041E, 0x043E)); // CYRILLIC CAPITAL LETTER O
  m.insert(charcharMapElement(0x041F, 0x043F)); // CYRILLIC CAPITAL LETTER PE
  m.insert(charcharMapElement(0x0420, 0x0440)); // CYRILLIC CAPITAL LETTER ER
  m.insert(charcharMapElement(0x0421, 0x0441)); // CYRILLIC CAPITAL LETTER ES
  m.insert(charcharMapElement(0x0422, 0x0442)); // CYRILLIC CAPITAL LETTER TE
  m.insert(charcharMapElement(0x0423, 0x0443)); // CYRILLIC CAPITAL LETTER U
  m.insert(charcharMapElement(0x0424, 0x0444)); // CYRILLIC CAPITAL LETTER EF
  m.insert(charcharMapElement(0x0425, 0x0445)); // CYRILLIC CAPITAL LETTER HA
  m.insert(charcharMapElement(0x0426, 0x0446)); // CYRILLIC CAPITAL LETTER TSE
  m.insert(charcharMapElement(0x0427, 0x0447)); // CYRILLIC CAPITAL LETTER CHE
  m.insert(charcharMapElement(0x0428, 0x0448)); // CYRILLIC CAPITAL LETTER SHA
  m.insert(charcharMapElement(0x0429, 0x0449)); // CYRILLIC CAPITAL LETTER SHCHA
  m.insert(charcharMapElement(0x042A, 0x044A)); // CYRILLIC CAPITAL LETTER HARD SIGN
  m.insert(charcharMapElement(0x042B, 0x044B)); // CYRILLIC CAPITAL LETTER YERU
  m.insert(charcharMapElement(0x042C, 0x044C)); // CYRILLIC CAPITAL LETTER SOFT SIGN
  m.insert(charcharMapElement(0x042D, 0x044D)); // CYRILLIC CAPITAL LETTER E
  m.insert(charcharMapElement(0x042E, 0x044E)); // CYRILLIC CAPITAL LETTER YU
  m.insert(charcharMapElement(0x042F, 0x044F)); // CYRILLIC CAPITAL LETTER YA
  m.insert(charcharMapElement(0x0460, 0x0461)); // CYRILLIC CAPITAL LETTER OMEGA
  m.insert(charcharMapElement(0x0462, 0x0463)); // CYRILLIC CAPITAL LETTER YAT
  m.insert(charcharMapElement(0x0464, 0x0465)); // CYRILLIC CAPITAL LETTER IOTIFIED E
  m.insert(charcharMapElement(0x0466, 0x0467)); // CYRILLIC CAPITAL LETTER LITTLE YUS
  m.insert(charcharMapElement(0x0468, 0x0469)); // CYRILLIC CAPITAL LETTER IOTIFIED LITTLE YUS
  m.insert(charcharMapElement(0x046A, 0x046B)); // CYRILLIC CAPITAL LETTER BIG YUS
  m.insert(charcharMapElement(0x046C, 0x046D)); // CYRILLIC CAPITAL LETTER IOTIFIED BIG YUS
  m.insert(charcharMapElement(0x046E, 0x046F)); // CYRILLIC CAPITAL LETTER KSI
  m.insert(charcharMapElement(0x0470, 0x0471)); // CYRILLIC CAPITAL LETTER PSI
  m.insert(charcharMapElement(0x0472, 0x0473)); // CYRILLIC CAPITAL LETTER FITA
  m.insert(charcharMapElement(0x0474, 0x0475)); // CYRILLIC CAPITAL LETTER IZHITSA
  m.insert(charcharMapElement(0x0476, 0x0477)); // CYRILLIC CAPITAL LETTER IZHITSA WITH DOUBLE GRAVE ACCENT
  m.insert(charcharMapElement(0x0478, 0x0479)); // CYRILLIC CAPITAL LETTER UK
  m.insert(charcharMapElement(0x047A, 0x047B)); // CYRILLIC CAPITAL LETTER ROUND OMEGA
  m.insert(charcharMapElement(0x047C, 0x047D)); // CYRILLIC CAPITAL LETTER OMEGA WITH TITLO
  m.insert(charcharMapElement(0x047E, 0x047F)); // CYRILLIC CAPITAL LETTER OT
  m.insert(charcharMapElement(0x0480, 0x0481)); // CYRILLIC CAPITAL LETTER KOPPA
  m.insert(charcharMapElement(0x048A, 0x048B)); // CYRILLIC CAPITAL LETTER SHORT I WITH TAIL
  m.insert(charcharMapElement(0x048C, 0x048D)); // CYRILLIC CAPITAL LETTER SEMISOFT SIGN
  m.insert(charcharMapElement(0x048E, 0x048F)); // CYRILLIC CAPITAL LETTER ER WITH TICK
  m.insert(charcharMapElement(0x0490, 0x0491)); // CYRILLIC CAPITAL LETTER GHE WITH UPTURN
  m.insert(charcharMapElement(0x0492, 0x0493)); // CYRILLIC CAPITAL LETTER GHE WITH STROKE
  m.insert(charcharMapElement(0x0494, 0x0495)); // CYRILLIC CAPITAL LETTER GHE WITH MIDDLE HOOK
  m.insert(charcharMapElement(0x0496, 0x0497)); // CYRILLIC CAPITAL LETTER ZHE WITH DESCENDER
  m.insert(charcharMapElement(0x0498, 0x0499)); // CYRILLIC CAPITAL LETTER ZE WITH DESCENDER
  m.insert(charcharMapElement(0x049A, 0x049B)); // CYRILLIC CAPITAL LETTER KA WITH DESCENDER
  m.insert(charcharMapElement(0x049C, 0x049D)); // CYRILLIC CAPITAL LETTER KA WITH VERTICAL STROKE
  m.insert(charcharMapElement(0x049E, 0x049F)); // CYRILLIC CAPITAL LETTER KA WITH STROKE
  m.insert(charcharMapElement(0x04A0, 0x04A1)); // CYRILLIC CAPITAL LETTER BASHKIR KA
  m.insert(charcharMapElement(0x04A2, 0x04A3)); // CYRILLIC CAPITAL LETTER EN WITH DESCENDER
  m.insert(charcharMapElement(0x04A4, 0x04A5)); // CYRILLIC CAPITAL LIGATURE EN GHE
  m.insert(charcharMapElement(0x04A6, 0x04A7)); // CYRILLIC CAPITAL LETTER PE WITH MIDDLE HOOK
  m.insert(charcharMapElement(0x04A8, 0x04A9)); // CYRILLIC CAPITAL LETTER ABKHASIAN HA
  m.insert(charcharMapElement(0x04AA, 0x04AB)); // CYRILLIC CAPITAL LETTER ES WITH DESCENDER
  m.insert(charcharMapElement(0x04AC, 0x04AD)); // CYRILLIC CAPITAL LETTER TE WITH DESCENDER
  m.insert(charcharMapElement(0x04AE, 0x04AF)); // CYRILLIC CAPITAL LETTER STRAIGHT U
  m.insert(charcharMapElement(0x04B0, 0x04B1)); // CYRILLIC CAPITAL LETTER STRAIGHT U WITH STROKE
  m.insert(charcharMapElement(0x04B2, 0x04B3)); // CYRILLIC CAPITAL LETTER HA WITH DESCENDER
  m.insert(charcharMapElement(0x04B4, 0x04B5)); // CYRILLIC CAPITAL LIGATURE TE TSE
  m.insert(charcharMapElement(0x04B6, 0x04B7)); // CYRILLIC CAPITAL LETTER CHE WITH DESCENDER
  m.insert(charcharMapElement(0x04B8, 0x04B9)); // CYRILLIC CAPITAL LETTER CHE WITH VERTICAL STROKE
  m.insert(charcharMapElement(0x04BA, 0x04BB)); // CYRILLIC CAPITAL LETTER SHHA
  m.insert(charcharMapElement(0x04BC, 0x04BD)); // CYRILLIC CAPITAL LETTER ABKHASIAN CHE
  m.insert(charcharMapElement(0x04BE, 0x04BF)); // CYRILLIC CAPITAL LETTER ABKHASIAN CHE WITH DESCENDER
  m.insert(charcharMapElement(0x04C0, 0x04CF)); // CYRILLIC LETTER PALOCHKA
  m.insert(charcharMapElement(0x04C1, 0x04C2)); // CYRILLIC CAPITAL LETTER ZHE WITH BREVE
  m.insert(charcharMapElement(0x04C3, 0x04C4)); // CYRILLIC CAPITAL LETTER KA WITH HOOK
  m.insert(charcharMapElement(0x04C5, 0x04C6)); // CYRILLIC CAPITAL LETTER EL WITH TAIL
  m.insert(charcharMapElement(0x04C7, 0x04C8)); // CYRILLIC CAPITAL LETTER EN WITH HOOK
  m.insert(charcharMapElement(0x04C9, 0x04CA)); // CYRILLIC CAPITAL LETTER EN WITH TAIL
  m.insert(charcharMapElement(0x04CB, 0x04CC)); // CYRILLIC CAPITAL LETTER KHAKASSIAN CHE
  m.insert(charcharMapElement(0x04CD, 0x04CE)); // CYRILLIC CAPITAL LETTER EM WITH TAIL
  m.insert(charcharMapElement(0x04D0, 0x04D1)); // CYRILLIC CAPITAL LETTER A WITH BREVE
  m.insert(charcharMapElement(0x04D2, 0x04D3)); // CYRILLIC CAPITAL LETTER A WITH DIAERESIS
  m.insert(charcharMapElement(0x04D4, 0x04D5)); // CYRILLIC CAPITAL LIGATURE A IE
  m.insert(charcharMapElement(0x04D6, 0x04D7)); // CYRILLIC CAPITAL LETTER IE WITH BREVE
  m.insert(charcharMapElement(0x04D8, 0x04D9)); // CYRILLIC CAPITAL LETTER SCHWA
  m.insert(charcharMapElement(0x04DA, 0x04DB)); // CYRILLIC CAPITAL LETTER SCHWA WITH DIAERESIS
  m.insert(charcharMapElement(0x04DC, 0x04DD)); // CYRILLIC CAPITAL LETTER ZHE WITH DIAERESIS
  m.insert(charcharMapElement(0x04DE, 0x04DF)); // CYRILLIC CAPITAL LETTER ZE WITH DIAERESIS
  m.insert(charcharMapElement(0x04E0, 0x04E1)); // CYRILLIC CAPITAL LETTER ABKHASIAN DZE
  m.insert(charcharMapElement(0x04E2, 0x04E3)); // CYRILLIC CAPITAL LETTER I WITH MACRON
  m.insert(charcharMapElement(0x04E4, 0x04E5)); // CYRILLIC CAPITAL LETTER I WITH DIAERESIS
  m.insert(charcharMapElement(0x04E6, 0x04E7)); // CYRILLIC CAPITAL LETTER O WITH DIAERESIS
  m.insert(charcharMapElement(0x04E8, 0x04E9)); // CYRILLIC CAPITAL LETTER BARRED O
  m.insert(charcharMapElement(0x04EA, 0x04EB)); // CYRILLIC CAPITAL LETTER BARRED O WITH DIAERESIS
  m.insert(charcharMapElement(0x04EC, 0x04ED)); // CYRILLIC CAPITAL LETTER E WITH DIAERESIS
  m.insert(charcharMapElement(0x04EE, 0x04EF)); // CYRILLIC CAPITAL LETTER U WITH MACRON
  m.insert(charcharMapElement(0x04F0, 0x04F1)); // CYRILLIC CAPITAL LETTER U WITH DIAERESIS
  m.insert(charcharMapElement(0x04F2, 0x04F3)); // CYRILLIC CAPITAL LETTER U WITH DOUBLE ACUTE
  m.insert(charcharMapElement(0x04F4, 0x04F5)); // CYRILLIC CAPITAL LETTER CHE WITH DIAERESIS
  m.insert(charcharMapElement(0x04F6, 0x04F7)); // CYRILLIC CAPITAL LETTER GHE WITH DESCENDER
  m.insert(charcharMapElement(0x04F8, 0x04F9)); // CYRILLIC CAPITAL LETTER YERU WITH DIAERESIS
  m.insert(charcharMapElement(0x04FA, 0x04FB)); // CYRILLIC CAPITAL LETTER GHE WITH STROKE AND HOOK
  m.insert(charcharMapElement(0x04FC, 0x04FD)); // CYRILLIC CAPITAL LETTER HA WITH HOOK
  m.insert(charcharMapElement(0x04FE, 0x04FF)); // CYRILLIC CAPITAL LETTER HA WITH STROKE
  m.insert(charcharMapElement(0x0500, 0x0501)); // CYRILLIC CAPITAL LETTER KOMI DE
  m.insert(charcharMapElement(0x0502, 0x0503)); // CYRILLIC CAPITAL LETTER KOMI DJE
  m.insert(charcharMapElement(0x0504, 0x0505)); // CYRILLIC CAPITAL LETTER KOMI ZJE
  m.insert(charcharMapElement(0x0506, 0x0507)); // CYRILLIC CAPITAL LETTER KOMI DZJE
  m.insert(charcharMapElement(0x0508, 0x0509)); // CYRILLIC CAPITAL LETTER KOMI LJE
  m.insert(charcharMapElement(0x050A, 0x050B)); // CYRILLIC CAPITAL LETTER KOMI NJE
  m.insert(charcharMapElement(0x050C, 0x050D)); // CYRILLIC CAPITAL LETTER KOMI SJE
  m.insert(charcharMapElement(0x050E, 0x050F)); // CYRILLIC CAPITAL LETTER KOMI TJE
  m.insert(charcharMapElement(0x0510, 0x0511)); // CYRILLIC CAPITAL LETTER REVERSED ZE
  m.insert(charcharMapElement(0x0512, 0x0513)); // CYRILLIC CAPITAL LETTER EL WITH HOOK
  m.insert(charcharMapElement(0x0514, 0x0515)); // CYRILLIC CAPITAL LETTER LHA
  m.insert(charcharMapElement(0x0516, 0x0517)); // CYRILLIC CAPITAL LETTER RHA
  m.insert(charcharMapElement(0x0518, 0x0519)); // CYRILLIC CAPITAL LETTER YAE
  m.insert(charcharMapElement(0x051A, 0x051B)); // CYRILLIC CAPITAL LETTER QA
  m.insert(charcharMapElement(0x051C, 0x051D)); // CYRILLIC CAPITAL LETTER WE
  m.insert(charcharMapElement(0x051E, 0x051F)); // CYRILLIC CAPITAL LETTER ALEUT KA
  m.insert(charcharMapElement(0x0520, 0x0521)); // CYRILLIC CAPITAL LETTER EL WITH MIDDLE HOOK
  m.insert(charcharMapElement(0x0522, 0x0523)); // CYRILLIC CAPITAL LETTER EN WITH MIDDLE HOOK
  m.insert(charcharMapElement(0x0524, 0x0525)); // CYRILLIC CAPITAL LETTER PE WITH DESCENDER
  m.insert(charcharMapElement(0x0526, 0x0527)); // CYRILLIC CAPITAL LETTER SHHA WITH DESCENDER
  m.insert(charcharMapElement(0x0531, 0x0561)); // ARMENIAN CAPITAL LETTER AYB
  m.insert(charcharMapElement(0x0532, 0x0562)); // ARMENIAN CAPITAL LETTER BEN
  m.insert(charcharMapElement(0x0533, 0x0563)); // ARMENIAN CAPITAL LETTER GIM
  m.insert(charcharMapElement(0x0534, 0x0564)); // ARMENIAN CAPITAL LETTER DA
  m.insert(charcharMapElement(0x0535, 0x0565)); // ARMENIAN CAPITAL LETTER ECH
  m.insert(charcharMapElement(0x0536, 0x0566)); // ARMENIAN CAPITAL LETTER ZA
  m.insert(charcharMapElement(0x0537, 0x0567)); // ARMENIAN CAPITAL LETTER EH
  m.insert(charcharMapElement(0x0538, 0x0568)); // ARMENIAN CAPITAL LETTER ET
  m.insert(charcharMapElement(0x0539, 0x0569)); // ARMENIAN CAPITAL LETTER TO
  m.insert(charcharMapElement(0x053A, 0x056A)); // ARMENIAN CAPITAL LETTER ZHE
  m.insert(charcharMapElement(0x053B, 0x056B)); // ARMENIAN CAPITAL LETTER INI
  m.insert(charcharMapElement(0x053C, 0x056C)); // ARMENIAN CAPITAL LETTER LIWN
  m.insert(charcharMapElement(0x053D, 0x056D)); // ARMENIAN CAPITAL LETTER XEH
  m.insert(charcharMapElement(0x053E, 0x056E)); // ARMENIAN CAPITAL LETTER CA
  m.insert(charcharMapElement(0x053F, 0x056F)); // ARMENIAN CAPITAL LETTER KEN
  m.insert(charcharMapElement(0x0540, 0x0570)); // ARMENIAN CAPITAL LETTER HO
  m.insert(charcharMapElement(0x0541, 0x0571)); // ARMENIAN CAPITAL LETTER JA
  m.insert(charcharMapElement(0x0542, 0x0572)); // ARMENIAN CAPITAL LETTER GHAD
  m.insert(charcharMapElement(0x0543, 0x0573)); // ARMENIAN CAPITAL LETTER CHEH
  m.insert(charcharMapElement(0x0544, 0x0574)); // ARMENIAN CAPITAL LETTER MEN
  m.insert(charcharMapElement(0x0545, 0x0575)); // ARMENIAN CAPITAL LETTER YI
  m.insert(charcharMapElement(0x0546, 0x0576)); // ARMENIAN CAPITAL LETTER NOW
  m.insert(charcharMapElement(0x0547, 0x0577)); // ARMENIAN CAPITAL LETTER SHA
  m.insert(charcharMapElement(0x0548, 0x0578)); // ARMENIAN CAPITAL LETTER VO
  m.insert(charcharMapElement(0x0549, 0x0579)); // ARMENIAN CAPITAL LETTER CHA
  m.insert(charcharMapElement(0x054A, 0x057A)); // ARMENIAN CAPITAL LETTER PEH
  m.insert(charcharMapElement(0x054B, 0x057B)); // ARMENIAN CAPITAL LETTER JHEH
  m.insert(charcharMapElement(0x054C, 0x057C)); // ARMENIAN CAPITAL LETTER RA
  m.insert(charcharMapElement(0x054D, 0x057D)); // ARMENIAN CAPITAL LETTER SEH
  m.insert(charcharMapElement(0x054E, 0x057E)); // ARMENIAN CAPITAL LETTER VEW
  m.insert(charcharMapElement(0x054F, 0x057F)); // ARMENIAN CAPITAL LETTER TIWN
  m.insert(charcharMapElement(0x0550, 0x0580)); // ARMENIAN CAPITAL LETTER REH
  m.insert(charcharMapElement(0x0551, 0x0581)); // ARMENIAN CAPITAL LETTER CO
  m.insert(charcharMapElement(0x0552, 0x0582)); // ARMENIAN CAPITAL LETTER YIWN
  m.insert(charcharMapElement(0x0553, 0x0583)); // ARMENIAN CAPITAL LETTER PIWR
  m.insert(charcharMapElement(0x0554, 0x0584)); // ARMENIAN CAPITAL LETTER KEH
  m.insert(charcharMapElement(0x0555, 0x0585)); // ARMENIAN CAPITAL LETTER OH
  m.insert(charcharMapElement(0x0556, 0x0586)); // ARMENIAN CAPITAL LETTER FEH
  m.insert(charcharMapElement(0x10A0, 0x2D00)); // GEORGIAN CAPITAL LETTER AN
  m.insert(charcharMapElement(0x10A1, 0x2D01)); // GEORGIAN CAPITAL LETTER BAN
  m.insert(charcharMapElement(0x10A2, 0x2D02)); // GEORGIAN CAPITAL LETTER GAN
  m.insert(charcharMapElement(0x10A3, 0x2D03)); // GEORGIAN CAPITAL LETTER DON
  m.insert(charcharMapElement(0x10A4, 0x2D04)); // GEORGIAN CAPITAL LETTER EN
  m.insert(charcharMapElement(0x10A5, 0x2D05)); // GEORGIAN CAPITAL LETTER VIN
  m.insert(charcharMapElement(0x10A6, 0x2D06)); // GEORGIAN CAPITAL LETTER ZEN
  m.insert(charcharMapElement(0x10A7, 0x2D07)); // GEORGIAN CAPITAL LETTER TAN
  m.insert(charcharMapElement(0x10A8, 0x2D08)); // GEORGIAN CAPITAL LETTER IN
  m.insert(charcharMapElement(0x10A9, 0x2D09)); // GEORGIAN CAPITAL LETTER KAN
  m.insert(charcharMapElement(0x10AA, 0x2D0A)); // GEORGIAN CAPITAL LETTER LAS
  m.insert(charcharMapElement(0x10AB, 0x2D0B)); // GEORGIAN CAPITAL LETTER MAN
  m.insert(charcharMapElement(0x10AC, 0x2D0C)); // GEORGIAN CAPITAL LETTER NAR
  m.insert(charcharMapElement(0x10AD, 0x2D0D)); // GEORGIAN CAPITAL LETTER ON
  m.insert(charcharMapElement(0x10AE, 0x2D0E)); // GEORGIAN CAPITAL LETTER PAR
  m.insert(charcharMapElement(0x10AF, 0x2D0F)); // GEORGIAN CAPITAL LETTER ZHAR
  m.insert(charcharMapElement(0x10B0, 0x2D10)); // GEORGIAN CAPITAL LETTER RAE
  m.insert(charcharMapElement(0x10B1, 0x2D11)); // GEORGIAN CAPITAL LETTER SAN
  m.insert(charcharMapElement(0x10B2, 0x2D12)); // GEORGIAN CAPITAL LETTER TAR
  m.insert(charcharMapElement(0x10B3, 0x2D13)); // GEORGIAN CAPITAL LETTER UN
  m.insert(charcharMapElement(0x10B4, 0x2D14)); // GEORGIAN CAPITAL LETTER PHAR
  m.insert(charcharMapElement(0x10B5, 0x2D15)); // GEORGIAN CAPITAL LETTER KHAR
  m.insert(charcharMapElement(0x10B6, 0x2D16)); // GEORGIAN CAPITAL LETTER GHAN
  m.insert(charcharMapElement(0x10B7, 0x2D17)); // GEORGIAN CAPITAL LETTER QAR
  m.insert(charcharMapElement(0x10B8, 0x2D18)); // GEORGIAN CAPITAL LETTER SHIN
  m.insert(charcharMapElement(0x10B9, 0x2D19)); // GEORGIAN CAPITAL LETTER CHIN
  m.insert(charcharMapElement(0x10BA, 0x2D1A)); // GEORGIAN CAPITAL LETTER CAN
  m.insert(charcharMapElement(0x10BB, 0x2D1B)); // GEORGIAN CAPITAL LETTER JIL
  m.insert(charcharMapElement(0x10BC, 0x2D1C)); // GEORGIAN CAPITAL LETTER CIL
  m.insert(charcharMapElement(0x10BD, 0x2D1D)); // GEORGIAN CAPITAL LETTER CHAR
  m.insert(charcharMapElement(0x10BE, 0x2D1E)); // GEORGIAN CAPITAL LETTER XAN
  m.insert(charcharMapElement(0x10BF, 0x2D1F)); // GEORGIAN CAPITAL LETTER JHAN
  m.insert(charcharMapElement(0x10C0, 0x2D20)); // GEORGIAN CAPITAL LETTER HAE
  m.insert(charcharMapElement(0x10C1, 0x2D21)); // GEORGIAN CAPITAL LETTER HE
  m.insert(charcharMapElement(0x10C2, 0x2D22)); // GEORGIAN CAPITAL LETTER HIE
  m.insert(charcharMapElement(0x10C3, 0x2D23)); // GEORGIAN CAPITAL LETTER WE
  m.insert(charcharMapElement(0x10C4, 0x2D24)); // GEORGIAN CAPITAL LETTER HAR
  m.insert(charcharMapElement(0x10C5, 0x2D25)); // GEORGIAN CAPITAL LETTER HOE
  m.insert(charcharMapElement(0x10C7, 0x2D27)); // GEORGIAN CAPITAL LETTER YN
  m.insert(charcharMapElement(0x10CD, 0x2D2D)); // GEORGIAN CAPITAL LETTER AEN
  m.insert(charcharMapElement(0x1E00, 0x1E01)); // LATIN CAPITAL LETTER A WITH RING BELOW
  m.insert(charcharMapElement(0x1E02, 0x1E03)); // LATIN CAPITAL LETTER B WITH DOT ABOVE
  m.insert(charcharMapElement(0x1E04, 0x1E05)); // LATIN CAPITAL LETTER B WITH DOT BELOW
  m.insert(charcharMapElement(0x1E06, 0x1E07)); // LATIN CAPITAL LETTER B WITH LINE BELOW
  m.insert(charcharMapElement(0x1E08, 0x1E09)); // LATIN CAPITAL LETTER C WITH CEDILLA AND ACUTE
  m.insert(charcharMapElement(0x1E0A, 0x1E0B)); // LATIN CAPITAL LETTER D WITH DOT ABOVE
  m.insert(charcharMapElement(0x1E0C, 0x1E0D)); // LATIN CAPITAL LETTER D WITH DOT BELOW
  m.insert(charcharMapElement(0x1E0E, 0x1E0F)); // LATIN CAPITAL LETTER D WITH LINE BELOW
  m.insert(charcharMapElement(0x1E10, 0x1E11)); // LATIN CAPITAL LETTER D WITH CEDILLA
  m.insert(charcharMapElement(0x1E12, 0x1E13)); // LATIN CAPITAL LETTER D WITH CIRCUMFLEX BELOW
  m.insert(charcharMapElement(0x1E14, 0x1E15)); // LATIN CAPITAL LETTER E WITH MACRON AND GRAVE
  m.insert(charcharMapElement(0x1E16, 0x1E17)); // LATIN CAPITAL LETTER E WITH MACRON AND ACUTE
  m.insert(charcharMapElement(0x1E18, 0x1E19)); // LATIN CAPITAL LETTER E WITH CIRCUMFLEX BELOW
  m.insert(charcharMapElement(0x1E1A, 0x1E1B)); // LATIN CAPITAL LETTER E WITH TILDE BELOW
  m.insert(charcharMapElement(0x1E1C, 0x1E1D)); // LATIN CAPITAL LETTER E WITH CEDILLA AND BREVE
  m.insert(charcharMapElement(0x1E1E, 0x1E1F)); // LATIN CAPITAL LETTER F WITH DOT ABOVE
  m.insert(charcharMapElement(0x1E20, 0x1E21)); // LATIN CAPITAL LETTER G WITH MACRON
  m.insert(charcharMapElement(0x1E22, 0x1E23)); // LATIN CAPITAL LETTER H WITH DOT ABOVE
  m.insert(charcharMapElement(0x1E24, 0x1E25)); // LATIN CAPITAL LETTER H WITH DOT BELOW
  m.insert(charcharMapElement(0x1E26, 0x1E27)); // LATIN CAPITAL LETTER H WITH DIAERESIS
  m.insert(charcharMapElement(0x1E28, 0x1E29)); // LATIN CAPITAL LETTER H WITH CEDILLA
  m.insert(charcharMapElement(0x1E2A, 0x1E2B)); // LATIN CAPITAL LETTER H WITH BREVE BELOW
  m.insert(charcharMapElement(0x1E2C, 0x1E2D)); // LATIN CAPITAL LETTER I WITH TILDE BELOW
  m.insert(charcharMapElement(0x1E2E, 0x1E2F)); // LATIN CAPITAL LETTER I WITH DIAERESIS AND ACUTE
  m.insert(charcharMapElement(0x1E30, 0x1E31)); // LATIN CAPITAL LETTER K WITH ACUTE
  m.insert(charcharMapElement(0x1E32, 0x1E33)); // LATIN CAPITAL LETTER K WITH DOT BELOW
  m.insert(charcharMapElement(0x1E34, 0x1E35)); // LATIN CAPITAL LETTER K WITH LINE BELOW
  m.insert(charcharMapElement(0x1E36, 0x1E37)); // LATIN CAPITAL LETTER L WITH DOT BELOW
  m.insert(charcharMapElement(0x1E38, 0x1E39)); // LATIN CAPITAL LETTER L WITH DOT BELOW AND MACRON
  m.insert(charcharMapElement(0x1E3A, 0x1E3B)); // LATIN CAPITAL LETTER L WITH LINE BELOW
  m.insert(charcharMapElement(0x1E3C, 0x1E3D)); // LATIN CAPITAL LETTER L WITH CIRCUMFLEX BELOW
  m.insert(charcharMapElement(0x1E3E, 0x1E3F)); // LATIN CAPITAL LETTER M WITH ACUTE
  m.insert(charcharMapElement(0x1E40, 0x1E41)); // LATIN CAPITAL LETTER M WITH DOT ABOVE
  m.insert(charcharMapElement(0x1E42, 0x1E43)); // LATIN CAPITAL LETTER M WITH DOT BELOW
  m.insert(charcharMapElement(0x1E44, 0x1E45)); // LATIN CAPITAL LETTER N WITH DOT ABOVE
  m.insert(charcharMapElement(0x1E46, 0x1E47)); // LATIN CAPITAL LETTER N WITH DOT BELOW
  m.insert(charcharMapElement(0x1E48, 0x1E49)); // LATIN CAPITAL LETTER N WITH LINE BELOW
  m.insert(charcharMapElement(0x1E4A, 0x1E4B)); // LATIN CAPITAL LETTER N WITH CIRCUMFLEX BELOW
  m.insert(charcharMapElement(0x1E4C, 0x1E4D)); // LATIN CAPITAL LETTER O WITH TILDE AND ACUTE
  m.insert(charcharMapElement(0x1E4E, 0x1E4F)); // LATIN CAPITAL LETTER O WITH TILDE AND DIAERESIS
  m.insert(charcharMapElement(0x1E50, 0x1E51)); // LATIN CAPITAL LETTER O WITH MACRON AND GRAVE
  m.insert(charcharMapElement(0x1E52, 0x1E53)); // LATIN CAPITAL LETTER O WITH MACRON AND ACUTE
  m.insert(charcharMapElement(0x1E54, 0x1E55)); // LATIN CAPITAL LETTER P WITH ACUTE
  m.insert(charcharMapElement(0x1E56, 0x1E57)); // LATIN CAPITAL LETTER P WITH DOT ABOVE
  m.insert(charcharMapElement(0x1E58, 0x1E59)); // LATIN CAPITAL LETTER R WITH DOT ABOVE
  m.insert(charcharMapElement(0x1E5A, 0x1E5B)); // LATIN CAPITAL LETTER R WITH DOT BELOW
  m.insert(charcharMapElement(0x1E5C, 0x1E5D)); // LATIN CAPITAL LETTER R WITH DOT BELOW AND MACRON
  m.insert(charcharMapElement(0x1E5E, 0x1E5F)); // LATIN CAPITAL LETTER R WITH LINE BELOW
  m.insert(charcharMapElement(0x1E60, 0x1E61)); // LATIN CAPITAL LETTER S WITH DOT ABOVE
  m.insert(charcharMapElement(0x1E62, 0x1E63)); // LATIN CAPITAL LETTER S WITH DOT BELOW
  m.insert(charcharMapElement(0x1E64, 0x1E65)); // LATIN CAPITAL LETTER S WITH ACUTE AND DOT ABOVE
  m.insert(charcharMapElement(0x1E66, 0x1E67)); // LATIN CAPITAL LETTER S WITH CARON AND DOT ABOVE
  m.insert(charcharMapElement(0x1E68, 0x1E69)); // LATIN CAPITAL LETTER S WITH DOT BELOW AND DOT ABOVE
  m.insert(charcharMapElement(0x1E6A, 0x1E6B)); // LATIN CAPITAL LETTER T WITH DOT ABOVE
  m.insert(charcharMapElement(0x1E6C, 0x1E6D)); // LATIN CAPITAL LETTER T WITH DOT BELOW
  m.insert(charcharMapElement(0x1E6E, 0x1E6F)); // LATIN CAPITAL LETTER T WITH LINE BELOW
  m.insert(charcharMapElement(0x1E70, 0x1E71)); // LATIN CAPITAL LETTER T WITH CIRCUMFLEX BELOW
  m.insert(charcharMapElement(0x1E72, 0x1E73)); // LATIN CAPITAL LETTER U WITH DIAERESIS BELOW
  m.insert(charcharMapElement(0x1E74, 0x1E75)); // LATIN CAPITAL LETTER U WITH TILDE BELOW
  m.insert(charcharMapElement(0x1E76, 0x1E77)); // LATIN CAPITAL LETTER U WITH CIRCUMFLEX BELOW
  m.insert(charcharMapElement(0x1E78, 0x1E79)); // LATIN CAPITAL LETTER U WITH TILDE AND ACUTE
  m.insert(charcharMapElement(0x1E7A, 0x1E7B)); // LATIN CAPITAL LETTER U WITH MACRON AND DIAERESIS
  m.insert(charcharMapElement(0x1E7C, 0x1E7D)); // LATIN CAPITAL LETTER V WITH TILDE
  m.insert(charcharMapElement(0x1E7E, 0x1E7F)); // LATIN CAPITAL LETTER V WITH DOT BELOW
  m.insert(charcharMapElement(0x1E80, 0x1E81)); // LATIN CAPITAL LETTER W WITH GRAVE
  m.insert(charcharMapElement(0x1E82, 0x1E83)); // LATIN CAPITAL LETTER W WITH ACUTE
  m.insert(charcharMapElement(0x1E84, 0x1E85)); // LATIN CAPITAL LETTER W WITH DIAERESIS
  m.insert(charcharMapElement(0x1E86, 0x1E87)); // LATIN CAPITAL LETTER W WITH DOT ABOVE
  m.insert(charcharMapElement(0x1E88, 0x1E89)); // LATIN CAPITAL LETTER W WITH DOT BELOW
  m.insert(charcharMapElement(0x1E8A, 0x1E8B)); // LATIN CAPITAL LETTER X WITH DOT ABOVE
  m.insert(charcharMapElement(0x1E8C, 0x1E8D)); // LATIN CAPITAL LETTER X WITH DIAERESIS
  m.insert(charcharMapElement(0x1E8E, 0x1E8F)); // LATIN CAPITAL LETTER Y WITH DOT ABOVE
  m.insert(charcharMapElement(0x1E90, 0x1E91)); // LATIN CAPITAL LETTER Z WITH CIRCUMFLEX
  m.insert(charcharMapElement(0x1E92, 0x1E93)); // LATIN CAPITAL LETTER Z WITH DOT BELOW
  m.insert(charcharMapElement(0x1E94, 0x1E95)); // LATIN CAPITAL LETTER Z WITH LINE BELOW
  m.insert(charcharMapElement(0x1E9B, 0x1E61)); // LATIN SMALL LETTER LONG S WITH DOT ABOVE
  m.insert(charcharMapElement(0x1EA0, 0x1EA1)); // LATIN CAPITAL LETTER A WITH DOT BELOW
  m.insert(charcharMapElement(0x1EA2, 0x1EA3)); // LATIN CAPITAL LETTER A WITH HOOK ABOVE
  m.insert(charcharMapElement(0x1EA4, 0x1EA5)); // LATIN CAPITAL LETTER A WITH CIRCUMFLEX AND ACUTE
  m.insert(charcharMapElement(0x1EA6, 0x1EA7)); // LATIN CAPITAL LETTER A WITH CIRCUMFLEX AND GRAVE
  m.insert(charcharMapElement(0x1EA8, 0x1EA9)); // LATIN CAPITAL LETTER A WITH CIRCUMFLEX AND HOOK ABOVE
  m.insert(charcharMapElement(0x1EAA, 0x1EAB)); // LATIN CAPITAL LETTER A WITH CIRCUMFLEX AND TILDE
  m.insert(charcharMapElement(0x1EAC, 0x1EAD)); // LATIN CAPITAL LETTER A WITH CIRCUMFLEX AND DOT BELOW
  m.insert(charcharMapElement(0x1EAE, 0x1EAF)); // LATIN CAPITAL LETTER A WITH BREVE AND ACUTE
  m.insert(charcharMapElement(0x1EB0, 0x1EB1)); // LATIN CAPITAL LETTER A WITH BREVE AND GRAVE
  m.insert(charcharMapElement(0x1EB2, 0x1EB3)); // LATIN CAPITAL LETTER A WITH BREVE AND HOOK ABOVE
  m.insert(charcharMapElement(0x1EB4, 0x1EB5)); // LATIN CAPITAL LETTER A WITH BREVE AND TILDE
  m.insert(charcharMapElement(0x1EB6, 0x1EB7)); // LATIN CAPITAL LETTER A WITH BREVE AND DOT BELOW
  m.insert(charcharMapElement(0x1EB8, 0x1EB9)); // LATIN CAPITAL LETTER E WITH DOT BELOW
  m.insert(charcharMapElement(0x1EBA, 0x1EBB)); // LATIN CAPITAL LETTER E WITH HOOK ABOVE
  m.insert(charcharMapElement(0x1EBC, 0x1EBD)); // LATIN CAPITAL LETTER E WITH TILDE
  m.insert(charcharMapElement(0x1EBE, 0x1EBF)); // LATIN CAPITAL LETTER E WITH CIRCUMFLEX AND ACUTE
  m.insert(charcharMapElement(0x1EC0, 0x1EC1)); // LATIN CAPITAL LETTER E WITH CIRCUMFLEX AND GRAVE
  m.insert(charcharMapElement(0x1EC2, 0x1EC3)); // LATIN CAPITAL LETTER E WITH CIRCUMFLEX AND HOOK ABOVE
  m.insert(charcharMapElement(0x1EC4, 0x1EC5)); // LATIN CAPITAL LETTER E WITH CIRCUMFLEX AND TILDE
  m.insert(charcharMapElement(0x1EC6, 0x1EC7)); // LATIN CAPITAL LETTER E WITH CIRCUMFLEX AND DOT BELOW
  m.insert(charcharMapElement(0x1EC8, 0x1EC9)); // LATIN CAPITAL LETTER I WITH HOOK ABOVE
  m.insert(charcharMapElement(0x1ECA, 0x1ECB)); // LATIN CAPITAL LETTER I WITH DOT BELOW
  m.insert(charcharMapElement(0x1ECC, 0x1ECD)); // LATIN CAPITAL LETTER O WITH DOT BELOW
  m.insert(charcharMapElement(0x1ECE, 0x1ECF)); // LATIN CAPITAL LETTER O WITH HOOK ABOVE
  m.insert(charcharMapElement(0x1ED0, 0x1ED1)); // LATIN CAPITAL LETTER O WITH CIRCUMFLEX AND ACUTE
  m.insert(charcharMapElement(0x1ED2, 0x1ED3)); // LATIN CAPITAL LETTER O WITH CIRCUMFLEX AND GRAVE
  m.insert(charcharMapElement(0x1ED4, 0x1ED5)); // LATIN CAPITAL LETTER O WITH CIRCUMFLEX AND HOOK ABOVE
  m.insert(charcharMapElement(0x1ED6, 0x1ED7)); // LATIN CAPITAL LETTER O WITH CIRCUMFLEX AND TILDE
  m.insert(charcharMapElement(0x1ED8, 0x1ED9)); // LATIN CAPITAL LETTER O WITH CIRCUMFLEX AND DOT BELOW
  m.insert(charcharMapElement(0x1EDA, 0x1EDB)); // LATIN CAPITAL LETTER O WITH HORN AND ACUTE
  m.insert(charcharMapElement(0x1EDC, 0x1EDD)); // LATIN CAPITAL LETTER O WITH HORN AND GRAVE
  m.insert(charcharMapElement(0x1EDE, 0x1EDF)); // LATIN CAPITAL LETTER O WITH HORN AND HOOK ABOVE
  m.insert(charcharMapElement(0x1EE0, 0x1EE1)); // LATIN CAPITAL LETTER O WITH HORN AND TILDE
  m.insert(charcharMapElement(0x1EE2, 0x1EE3)); // LATIN CAPITAL LETTER O WITH HORN AND DOT BELOW
  m.insert(charcharMapElement(0x1EE4, 0x1EE5)); // LATIN CAPITAL LETTER U WITH DOT BELOW
  m.insert(charcharMapElement(0x1EE6, 0x1EE7)); // LATIN CAPITAL LETTER U WITH HOOK ABOVE
  m.insert(charcharMapElement(0x1EE8, 0x1EE9)); // LATIN CAPITAL LETTER U WITH HORN AND ACUTE
  m.insert(charcharMapElement(0x1EEA, 0x1EEB)); // LATIN CAPITAL LETTER U WITH HORN AND GRAVE
  m.insert(charcharMapElement(0x1EEC, 0x1EED)); // LATIN CAPITAL LETTER U WITH HORN AND HOOK ABOVE
  m.insert(charcharMapElement(0x1EEE, 0x1EEF)); // LATIN CAPITAL LETTER U WITH HORN AND TILDE
  m.insert(charcharMapElement(0x1EF0, 0x1EF1)); // LATIN CAPITAL LETTER U WITH HORN AND DOT BELOW
  m.insert(charcharMapElement(0x1EF2, 0x1EF3)); // LATIN CAPITAL LETTER Y WITH GRAVE
  m.insert(charcharMapElement(0x1EF4, 0x1EF5)); // LATIN CAPITAL LETTER Y WITH DOT BELOW
  m.insert(charcharMapElement(0x1EF6, 0x1EF7)); // LATIN CAPITAL LETTER Y WITH HOOK ABOVE
  m.insert(charcharMapElement(0x1EF8, 0x1EF9)); // LATIN CAPITAL LETTER Y WITH TILDE
  m.insert(charcharMapElement(0x1EFA, 0x1EFB)); // LATIN CAPITAL LETTER MIDDLE-WELSH LL
  m.insert(charcharMapElement(0x1EFC, 0x1EFD)); // LATIN CAPITAL LETTER MIDDLE-WELSH V
  m.insert(charcharMapElement(0x1EFE, 0x1EFF)); // LATIN CAPITAL LETTER Y WITH LOOP
  m.insert(charcharMapElement(0x1F08, 0x1F00)); // GREEK CAPITAL LETTER ALPHA WITH PSILI
  m.insert(charcharMapElement(0x1F09, 0x1F01)); // GREEK CAPITAL LETTER ALPHA WITH DASIA
  m.insert(charcharMapElement(0x1F0A, 0x1F02)); // GREEK CAPITAL LETTER ALPHA WITH PSILI AND VARIA
  m.insert(charcharMapElement(0x1F0B, 0x1F03)); // GREEK CAPITAL LETTER ALPHA WITH DASIA AND VARIA
  m.insert(charcharMapElement(0x1F0C, 0x1F04)); // GREEK CAPITAL LETTER ALPHA WITH PSILI AND OXIA
  m.insert(charcharMapElement(0x1F0D, 0x1F05)); // GREEK CAPITAL LETTER ALPHA WITH DASIA AND OXIA
  m.insert(charcharMapElement(0x1F0E, 0x1F06)); // GREEK CAPITAL LETTER ALPHA WITH PSILI AND PERISPOMENI
  m.insert(charcharMapElement(0x1F0F, 0x1F07)); // GREEK CAPITAL LETTER ALPHA WITH DASIA AND PERISPOMENI
  m.insert(charcharMapElement(0x1F18, 0x1F10)); // GREEK CAPITAL LETTER EPSILON WITH PSILI
  m.insert(charcharMapElement(0x1F19, 0x1F11)); // GREEK CAPITAL LETTER EPSILON WITH DASIA
  m.insert(charcharMapElement(0x1F1A, 0x1F12)); // GREEK CAPITAL LETTER EPSILON WITH PSILI AND VARIA
  m.insert(charcharMapElement(0x1F1B, 0x1F13)); // GREEK CAPITAL LETTER EPSILON WITH DASIA AND VARIA
  m.insert(charcharMapElement(0x1F1C, 0x1F14)); // GREEK CAPITAL LETTER EPSILON WITH PSILI AND OXIA
  m.insert(charcharMapElement(0x1F1D, 0x1F15)); // GREEK CAPITAL LETTER EPSILON WITH DASIA AND OXIA
  m.insert(charcharMapElement(0x1F28, 0x1F20)); // GREEK CAPITAL LETTER ETA WITH PSILI
  m.insert(charcharMapElement(0x1F29, 0x1F21)); // GREEK CAPITAL LETTER ETA WITH DASIA
  m.insert(charcharMapElement(0x1F2A, 0x1F22)); // GREEK CAPITAL LETTER ETA WITH PSILI AND VARIA
  m.insert(charcharMapElement(0x1F2B, 0x1F23)); // GREEK CAPITAL LETTER ETA WITH DASIA AND VARIA
  m.insert(charcharMapElement(0x1F2C, 0x1F24)); // GREEK CAPITAL LETTER ETA WITH PSILI AND OXIA
  m.insert(charcharMapElement(0x1F2D, 0x1F25)); // GREEK CAPITAL LETTER ETA WITH DASIA AND OXIA
  m.insert(charcharMapElement(0x1F2E, 0x1F26)); // GREEK CAPITAL LETTER ETA WITH PSILI AND PERISPOMENI
  m.insert(charcharMapElement(0x1F2F, 0x1F27)); // GREEK CAPITAL LETTER ETA WITH DASIA AND PERISPOMENI
  m.insert(charcharMapElement(0x1F38, 0x1F30)); // GREEK CAPITAL LETTER IOTA WITH PSILI
  m.insert(charcharMapElement(0x1F39, 0x1F31)); // GREEK CAPITAL LETTER IOTA WITH DASIA
  m.insert(charcharMapElement(0x1F3A, 0x1F32)); // GREEK CAPITAL LETTER IOTA WITH PSILI AND VARIA
  m.insert(charcharMapElement(0x1F3B, 0x1F33)); // GREEK CAPITAL LETTER IOTA WITH DASIA AND VARIA
  m.insert(charcharMapElement(0x1F3C, 0x1F34)); // GREEK CAPITAL LETTER IOTA WITH PSILI AND OXIA
  m.insert(charcharMapElement(0x1F3D, 0x1F35)); // GREEK CAPITAL LETTER IOTA WITH DASIA AND OXIA
  m.insert(charcharMapElement(0x1F3E, 0x1F36)); // GREEK CAPITAL LETTER IOTA WITH PSILI AND PERISPOMENI
  m.insert(charcharMapElement(0x1F3F, 0x1F37)); // GREEK CAPITAL LETTER IOTA WITH DASIA AND PERISPOMENI
  m.insert(charcharMapElement(0x1F48, 0x1F40)); // GREEK CAPITAL LETTER OMICRON WITH PSILI
  m.insert(charcharMapElement(0x1F49, 0x1F41)); // GREEK CAPITAL LETTER OMICRON WITH DASIA
  m.insert(charcharMapElement(0x1F4A, 0x1F42)); // GREEK CAPITAL LETTER OMICRON WITH PSILI AND VARIA
  m.insert(charcharMapElement(0x1F4B, 0x1F43)); // GREEK CAPITAL LETTER OMICRON WITH DASIA AND VARIA
  m.insert(charcharMapElement(0x1F4C, 0x1F44)); // GREEK CAPITAL LETTER OMICRON WITH PSILI AND OXIA
  m.insert(charcharMapElement(0x1F4D, 0x1F45)); // GREEK CAPITAL LETTER OMICRON WITH DASIA AND OXIA
  m.insert(charcharMapElement(0x1F59, 0x1F51)); // GREEK CAPITAL LETTER UPSILON WITH DASIA
  m.insert(charcharMapElement(0x1F5B, 0x1F53)); // GREEK CAPITAL LETTER UPSILON WITH DASIA AND VARIA
  m.insert(charcharMapElement(0x1F5D, 0x1F55)); // GREEK CAPITAL LETTER UPSILON WITH DASIA AND OXIA
  m.insert(charcharMapElement(0x1F5F, 0x1F57)); // GREEK CAPITAL LETTER UPSILON WITH DASIA AND PERISPOMENI
  m.insert(charcharMapElement(0x1F68, 0x1F60)); // GREEK CAPITAL LETTER OMEGA WITH PSILI
  m.insert(charcharMapElement(0x1F69, 0x1F61)); // GREEK CAPITAL LETTER OMEGA WITH DASIA
  m.insert(charcharMapElement(0x1F6A, 0x1F62)); // GREEK CAPITAL LETTER OMEGA WITH PSILI AND VARIA
  m.insert(charcharMapElement(0x1F6B, 0x1F63)); // GREEK CAPITAL LETTER OMEGA WITH DASIA AND VARIA
  m.insert(charcharMapElement(0x1F6C, 0x1F64)); // GREEK CAPITAL LETTER OMEGA WITH PSILI AND OXIA
  m.insert(charcharMapElement(0x1F6D, 0x1F65)); // GREEK CAPITAL LETTER OMEGA WITH DASIA AND OXIA
  m.insert(charcharMapElement(0x1F6E, 0x1F66)); // GREEK CAPITAL LETTER OMEGA WITH PSILI AND PERISPOMENI
  m.insert(charcharMapElement(0x1F6F, 0x1F67)); // GREEK CAPITAL LETTER OMEGA WITH DASIA AND PERISPOMENI
  m.insert(charcharMapElement(0x1FB8, 0x1FB0)); // GREEK CAPITAL LETTER ALPHA WITH VRACHY
  m.insert(charcharMapElement(0x1FB9, 0x1FB1)); // GREEK CAPITAL LETTER ALPHA WITH MACRON
  m.insert(charcharMapElement(0x1FBA, 0x1F70)); // GREEK CAPITAL LETTER ALPHA WITH VARIA
  m.insert(charcharMapElement(0x1FBB, 0x1F71)); // GREEK CAPITAL LETTER ALPHA WITH OXIA
  m.insert(charcharMapElement(0x1FBE, 0x03B9)); // GREEK PROSGEGRAMMENI
  m.insert(charcharMapElement(0x1FC8, 0x1F72)); // GREEK CAPITAL LETTER EPSILON WITH VARIA
  m.insert(charcharMapElement(0x1FC9, 0x1F73)); // GREEK CAPITAL LETTER EPSILON WITH OXIA
  m.insert(charcharMapElement(0x1FCA, 0x1F74)); // GREEK CAPITAL LETTER ETA WITH VARIA
  m.insert(charcharMapElement(0x1FCB, 0x1F75)); // GREEK CAPITAL LETTER ETA WITH OXIA
  m.insert(charcharMapElement(0x1FD8, 0x1FD0)); // GREEK CAPITAL LETTER IOTA WITH VRACHY
  m.insert(charcharMapElement(0x1FD9, 0x1FD1)); // GREEK CAPITAL LETTER IOTA WITH MACRON
  m.insert(charcharMapElement(0x1FDA, 0x1F76)); // GREEK CAPITAL LETTER IOTA WITH VARIA
  m.insert(charcharMapElement(0x1FDB, 0x1F77)); // GREEK CAPITAL LETTER IOTA WITH OXIA
  m.insert(charcharMapElement(0x1FE8, 0x1FE0)); // GREEK CAPITAL LETTER UPSILON WITH VRACHY
  m.insert(charcharMapElement(0x1FE9, 0x1FE1)); // GREEK CAPITAL LETTER UPSILON WITH MACRON
  m.insert(charcharMapElement(0x1FEA, 0x1F7A)); // GREEK CAPITAL LETTER UPSILON WITH VARIA
  m.insert(charcharMapElement(0x1FEB, 0x1F7B)); // GREEK CAPITAL LETTER UPSILON WITH OXIA
  m.insert(charcharMapElement(0x1FEC, 0x1FE5)); // GREEK CAPITAL LETTER RHO WITH DASIA
  m.insert(charcharMapElement(0x1FF8, 0x1F78)); // GREEK CAPITAL LETTER OMICRON WITH VARIA
  m.insert(charcharMapElement(0x1FF9, 0x1F79)); // GREEK CAPITAL LETTER OMICRON WITH OXIA
  m.insert(charcharMapElement(0x1FFA, 0x1F7C)); // GREEK CAPITAL LETTER OMEGA WITH VARIA
  m.insert(charcharMapElement(0x1FFB, 0x1F7D)); // GREEK CAPITAL LETTER OMEGA WITH OXIA
  m.insert(charcharMapElement(0x2126, 0x03C9)); // OHM SIGN
  m.insert(charcharMapElement(0x212A, 0x006B)); // KELVIN SIGN
  m.insert(charcharMapElement(0x212B, 0x00E5)); // ANGSTROM SIGN
  m.insert(charcharMapElement(0x2132, 0x214E)); // TURNED CAPITAL F
  m.insert(charcharMapElement(0x2160, 0x2170)); // ROMAN NUMERAL ONE
  m.insert(charcharMapElement(0x2161, 0x2171)); // ROMAN NUMERAL TWO
  m.insert(charcharMapElement(0x2162, 0x2172)); // ROMAN NUMERAL THREE
  m.insert(charcharMapElement(0x2163, 0x2173)); // ROMAN NUMERAL FOUR
  m.insert(charcharMapElement(0x2164, 0x2174)); // ROMAN NUMERAL FIVE
  m.insert(charcharMapElement(0x2165, 0x2175)); // ROMAN NUMERAL SIX
  m.insert(charcharMapElement(0x2166, 0x2176)); // ROMAN NUMERAL SEVEN
  m.insert(charcharMapElement(0x2167, 0x2177)); // ROMAN NUMERAL EIGHT
  m.insert(charcharMapElement(0x2168, 0x2178)); // ROMAN NUMERAL NINE
  m.insert(charcharMapElement(0x2169, 0x2179)); // ROMAN NUMERAL TEN
  m.insert(charcharMapElement(0x216A, 0x217A)); // ROMAN NUMERAL ELEVEN
  m.insert(charcharMapElement(0x216B, 0x217B)); // ROMAN NUMERAL TWELVE
  m.insert(charcharMapElement(0x216C, 0x217C)); // ROMAN NUMERAL FIFTY
  m.insert(charcharMapElement(0x216D, 0x217D)); // ROMAN NUMERAL ONE HUNDRED
  m.insert(charcharMapElement(0x216E, 0x217E)); // ROMAN NUMERAL FIVE HUNDRED
  m.insert(charcharMapElement(0x216F, 0x217F)); // ROMAN NUMERAL ONE THOUSAND
  m.insert(charcharMapElement(0x2183, 0x2184)); // ROMAN NUMERAL REVERSED ONE HUNDRED
  m.insert(charcharMapElement(0x24B6, 0x24D0)); // CIRCLED LATIN CAPITAL LETTER A
  m.insert(charcharMapElement(0x24B7, 0x24D1)); // CIRCLED LATIN CAPITAL LETTER B
  m.insert(charcharMapElement(0x24B8, 0x24D2)); // CIRCLED LATIN CAPITAL LETTER C
  m.insert(charcharMapElement(0x24B9, 0x24D3)); // CIRCLED LATIN CAPITAL LETTER D
  m.insert(charcharMapElement(0x24BA, 0x24D4)); // CIRCLED LATIN CAPITAL LETTER E
  m.insert(charcharMapElement(0x24BB, 0x24D5)); // CIRCLED LATIN CAPITAL LETTER F
  m.insert(charcharMapElement(0x24BC, 0x24D6)); // CIRCLED LATIN CAPITAL LETTER G
  m.insert(charcharMapElement(0x24BD, 0x24D7)); // CIRCLED LATIN CAPITAL LETTER H
  m.insert(charcharMapElement(0x24BE, 0x24D8)); // CIRCLED LATIN CAPITAL LETTER I
  m.insert(charcharMapElement(0x24BF, 0x24D9)); // CIRCLED LATIN CAPITAL LETTER J
  m.insert(charcharMapElement(0x24C0, 0x24DA)); // CIRCLED LATIN CAPITAL LETTER K
  m.insert(charcharMapElement(0x24C1, 0x24DB)); // CIRCLED LATIN CAPITAL LETTER L
  m.insert(charcharMapElement(0x24C2, 0x24DC)); // CIRCLED LATIN CAPITAL LETTER M
  m.insert(charcharMapElement(0x24C3, 0x24DD)); // CIRCLED LATIN CAPITAL LETTER N
  m.insert(charcharMapElement(0x24C4, 0x24DE)); // CIRCLED LATIN CAPITAL LETTER O
  m.insert(charcharMapElement(0x24C5, 0x24DF)); // CIRCLED LATIN CAPITAL LETTER P
  m.insert(charcharMapElement(0x24C6, 0x24E0)); // CIRCLED LATIN CAPITAL LETTER Q
  m.insert(charcharMapElement(0x24C7, 0x24E1)); // CIRCLED LATIN CAPITAL LETTER R
  m.insert(charcharMapElement(0x24C8, 0x24E2)); // CIRCLED LATIN CAPITAL LETTER S
  m.insert(charcharMapElement(0x24C9, 0x24E3)); // CIRCLED LATIN CAPITAL LETTER T
  m.insert(charcharMapElement(0x24CA, 0x24E4)); // CIRCLED LATIN CAPITAL LETTER U
  m.insert(charcharMapElement(0x24CB, 0x24E5)); // CIRCLED LATIN CAPITAL LETTER V
  m.insert(charcharMapElement(0x24CC, 0x24E6)); // CIRCLED LATIN CAPITAL LETTER W
  m.insert(charcharMapElement(0x24CD, 0x24E7)); // CIRCLED LATIN CAPITAL LETTER X
  m.insert(charcharMapElement(0x24CE, 0x24E8)); // CIRCLED LATIN CAPITAL LETTER Y
  m.insert(charcharMapElement(0x24CF, 0x24E9)); // CIRCLED LATIN CAPITAL LETTER Z
  m.insert(charcharMapElement(0x2C00, 0x2C30)); // GLAGOLITIC CAPITAL LETTER AZU
  m.insert(charcharMapElement(0x2C01, 0x2C31)); // GLAGOLITIC CAPITAL LETTER BUKY
  m.insert(charcharMapElement(0x2C02, 0x2C32)); // GLAGOLITIC CAPITAL LETTER VEDE
  m.insert(charcharMapElement(0x2C03, 0x2C33)); // GLAGOLITIC CAPITAL LETTER GLAGOLI
  m.insert(charcharMapElement(0x2C04, 0x2C34)); // GLAGOLITIC CAPITAL LETTER DOBRO
  m.insert(charcharMapElement(0x2C05, 0x2C35)); // GLAGOLITIC CAPITAL LETTER YESTU
  m.insert(charcharMapElement(0x2C06, 0x2C36)); // GLAGOLITIC CAPITAL LETTER ZHIVETE
  m.insert(charcharMapElement(0x2C07, 0x2C37)); // GLAGOLITIC CAPITAL LETTER DZELO
  m.insert(charcharMapElement(0x2C08, 0x2C38)); // GLAGOLITIC CAPITAL LETTER ZEMLJA
  m.insert(charcharMapElement(0x2C09, 0x2C39)); // GLAGOLITIC CAPITAL LETTER IZHE
  m.insert(charcharMapElement(0x2C0A, 0x2C3A)); // GLAGOLITIC CAPITAL LETTER INITIAL IZHE
  m.insert(charcharMapElement(0x2C0B, 0x2C3B)); // GLAGOLITIC CAPITAL LETTER I
  m.insert(charcharMapElement(0x2C0C, 0x2C3C)); // GLAGOLITIC CAPITAL LETTER DJERVI
  m.insert(charcharMapElement(0x2C0D, 0x2C3D)); // GLAGOLITIC CAPITAL LETTER KAKO
  m.insert(charcharMapElement(0x2C0E, 0x2C3E)); // GLAGOLITIC CAPITAL LETTER LJUDIJE
  m.insert(charcharMapElement(0x2C0F, 0x2C3F)); // GLAGOLITIC CAPITAL LETTER MYSLITE
  m.insert(charcharMapElement(0x2C10, 0x2C40)); // GLAGOLITIC CAPITAL LETTER NASHI
  m.insert(charcharMapElement(0x2C11, 0x2C41)); // GLAGOLITIC CAPITAL LETTER ONU
  m.insert(charcharMapElement(0x2C12, 0x2C42)); // GLAGOLITIC CAPITAL LETTER POKOJI
  m.insert(charcharMapElement(0x2C13, 0x2C43)); // GLAGOLITIC CAPITAL LETTER RITSI
  m.insert(charcharMapElement(0x2C14, 0x2C44)); // GLAGOLITIC CAPITAL LETTER SLOVO
  m.insert(charcharMapElement(0x2C15, 0x2C45)); // GLAGOLITIC CAPITAL LETTER TVRIDO
  m.insert(charcharMapElement(0x2C16, 0x2C46)); // GLAGOLITIC CAPITAL LETTER UKU
  m.insert(charcharMapElement(0x2C17, 0x2C47)); // GLAGOLITIC CAPITAL LETTER FRITU
  m.insert(charcharMapElement(0x2C18, 0x2C48)); // GLAGOLITIC CAPITAL LETTER HERU
  m.insert(charcharMapElement(0x2C19, 0x2C49)); // GLAGOLITIC CAPITAL LETTER OTU
  m.insert(charcharMapElement(0x2C1A, 0x2C4A)); // GLAGOLITIC CAPITAL LETTER PE
  m.insert(charcharMapElement(0x2C1B, 0x2C4B)); // GLAGOLITIC CAPITAL LETTER SHTA
  m.insert(charcharMapElement(0x2C1C, 0x2C4C)); // GLAGOLITIC CAPITAL LETTER TSI
  m.insert(charcharMapElement(0x2C1D, 0x2C4D)); // GLAGOLITIC CAPITAL LETTER CHRIVI
  m.insert(charcharMapElement(0x2C1E, 0x2C4E)); // GLAGOLITIC CAPITAL LETTER SHA
  m.insert(charcharMapElement(0x2C1F, 0x2C4F)); // GLAGOLITIC CAPITAL LETTER YERU
  m.insert(charcharMapElement(0x2C20, 0x2C50)); // GLAGOLITIC CAPITAL LETTER YERI
  m.insert(charcharMapElement(0x2C21, 0x2C51)); // GLAGOLITIC CAPITAL LETTER YATI
  m.insert(charcharMapElement(0x2C22, 0x2C52)); // GLAGOLITIC CAPITAL LETTER SPIDERY HA
  m.insert(charcharMapElement(0x2C23, 0x2C53)); // GLAGOLITIC CAPITAL LETTER YU
  m.insert(charcharMapElement(0x2C24, 0x2C54)); // GLAGOLITIC CAPITAL LETTER SMALL YUS
  m.insert(charcharMapElement(0x2C25, 0x2C55)); // GLAGOLITIC CAPITAL LETTER SMALL YUS WITH TAIL
  m.insert(charcharMapElement(0x2C26, 0x2C56)); // GLAGOLITIC CAPITAL LETTER YO
  m.insert(charcharMapElement(0x2C27, 0x2C57)); // GLAGOLITIC CAPITAL LETTER IOTATED SMALL YUS
  m.insert(charcharMapElement(0x2C28, 0x2C58)); // GLAGOLITIC CAPITAL LETTER BIG YUS
  m.insert(charcharMapElement(0x2C29, 0x2C59)); // GLAGOLITIC CAPITAL LETTER IOTATED BIG YUS
  m.insert(charcharMapElement(0x2C2A, 0x2C5A)); // GLAGOLITIC CAPITAL LETTER FITA
  m.insert(charcharMapElement(0x2C2B, 0x2C5B)); // GLAGOLITIC CAPITAL LETTER IZHITSA
  m.insert(charcharMapElement(0x2C2C, 0x2C5C)); // GLAGOLITIC CAPITAL LETTER SHTAPIC
  m.insert(charcharMapElement(0x2C2D, 0x2C5D)); // GLAGOLITIC CAPITAL LETTER TROKUTASTI A
  m.insert(charcharMapElement(0x2C2E, 0x2C5E)); // GLAGOLITIC CAPITAL LETTER LATINATE MYSLITE
  m.insert(charcharMapElement(0x2C60, 0x2C61)); // LATIN CAPITAL LETTER L WITH DOUBLE BAR
  m.insert(charcharMapElement(0x2C62, 0x026B)); // LATIN CAPITAL LETTER L WITH MIDDLE TILDE
  m.insert(charcharMapElement(0x2C63, 0x1D7D)); // LATIN CAPITAL LETTER P WITH STROKE
  m.insert(charcharMapElement(0x2C64, 0x027D)); // LATIN CAPITAL LETTER R WITH TAIL
  m.insert(charcharMapElement(0x2C67, 0x2C68)); // LATIN CAPITAL LETTER H WITH DESCENDER
  m.insert(charcharMapElement(0x2C69, 0x2C6A)); // LATIN CAPITAL LETTER K WITH DESCENDER
  m.insert(charcharMapElement(0x2C6B, 0x2C6C)); // LATIN CAPITAL LETTER Z WITH DESCENDER
  m.insert(charcharMapElement(0x2C6D, 0x0251)); // LATIN CAPITAL LETTER ALPHA
  m.insert(charcharMapElement(0x2C6E, 0x0271)); // LATIN CAPITAL LETTER M WITH HOOK
  m.insert(charcharMapElement(0x2C6F, 0x0250)); // LATIN CAPITAL LETTER TURNED A
  m.insert(charcharMapElement(0x2C70, 0x0252)); // LATIN CAPITAL LETTER TURNED ALPHA
  m.insert(charcharMapElement(0x2C72, 0x2C73)); // LATIN CAPITAL LETTER W WITH HOOK
  m.insert(charcharMapElement(0x2C75, 0x2C76)); // LATIN CAPITAL LETTER HALF H
  m.insert(charcharMapElement(0x2C7E, 0x023F)); // LATIN CAPITAL LETTER S WITH SWASH TAIL
  m.insert(charcharMapElement(0x2C7F, 0x0240)); // LATIN CAPITAL LETTER Z WITH SWASH TAIL
  m.insert(charcharMapElement(0x2C80, 0x2C81)); // COPTIC CAPITAL LETTER ALFA
  m.insert(charcharMapElement(0x2C82, 0x2C83)); // COPTIC CAPITAL LETTER VIDA
  m.insert(charcharMapElement(0x2C84, 0x2C85)); // COPTIC CAPITAL LETTER GAMMA
  m.insert(charcharMapElement(0x2C86, 0x2C87)); // COPTIC CAPITAL LETTER DALDA
  m.insert(charcharMapElement(0x2C88, 0x2C89)); // COPTIC CAPITAL LETTER EIE
  m.insert(charcharMapElement(0x2C8A, 0x2C8B)); // COPTIC CAPITAL LETTER SOU
  m.insert(charcharMapElement(0x2C8C, 0x2C8D)); // COPTIC CAPITAL LETTER ZATA
  m.insert(charcharMapElement(0x2C8E, 0x2C8F)); // COPTIC CAPITAL LETTER HATE
  m.insert(charcharMapElement(0x2C90, 0x2C91)); // COPTIC CAPITAL LETTER THETHE
  m.insert(charcharMapElement(0x2C92, 0x2C93)); // COPTIC CAPITAL LETTER IAUDA
  m.insert(charcharMapElement(0x2C94, 0x2C95)); // COPTIC CAPITAL LETTER KAPA
  m.insert(charcharMapElement(0x2C96, 0x2C97)); // COPTIC CAPITAL LETTER LAULA
  m.insert(charcharMapElement(0x2C98, 0x2C99)); // COPTIC CAPITAL LETTER MI
  m.insert(charcharMapElement(0x2C9A, 0x2C9B)); // COPTIC CAPITAL LETTER NI
  m.insert(charcharMapElement(0x2C9C, 0x2C9D)); // COPTIC CAPITAL LETTER KSI
  m.insert(charcharMapElement(0x2C9E, 0x2C9F)); // COPTIC CAPITAL LETTER O
  m.insert(charcharMapElement(0x2CA0, 0x2CA1)); // COPTIC CAPITAL LETTER PI
  m.insert(charcharMapElement(0x2CA2, 0x2CA3)); // COPTIC CAPITAL LETTER RO
  m.insert(charcharMapElement(0x2CA4, 0x2CA5)); // COPTIC CAPITAL LETTER SIMA
  m.insert(charcharMapElement(0x2CA6, 0x2CA7)); // COPTIC CAPITAL LETTER TAU
  m.insert(charcharMapElement(0x2CA8, 0x2CA9)); // COPTIC CAPITAL LETTER UA
  m.insert(charcharMapElement(0x2CAA, 0x2CAB)); // COPTIC CAPITAL LETTER FI
  m.insert(charcharMapElement(0x2CAC, 0x2CAD)); // COPTIC CAPITAL LETTER KHI
  m.insert(charcharMapElement(0x2CAE, 0x2CAF)); // COPTIC CAPITAL LETTER PSI
  m.insert(charcharMapElement(0x2CB0, 0x2CB1)); // COPTIC CAPITAL LETTER OOU
  m.insert(charcharMapElement(0x2CB2, 0x2CB3)); // COPTIC CAPITAL LETTER DIALECT-P ALEF
  m.insert(charcharMapElement(0x2CB4, 0x2CB5)); // COPTIC CAPITAL LETTER OLD COPTIC AIN
  m.insert(charcharMapElement(0x2CB6, 0x2CB7)); // COPTIC CAPITAL LETTER CRYPTOGRAMMIC EIE
  m.insert(charcharMapElement(0x2CB8, 0x2CB9)); // COPTIC CAPITAL LETTER DIALECT-P KAPA
  m.insert(charcharMapElement(0x2CBA, 0x2CBB)); // COPTIC CAPITAL LETTER DIALECT-P NI
  m.insert(charcharMapElement(0x2CBC, 0x2CBD)); // COPTIC CAPITAL LETTER CRYPTOGRAMMIC NI
  m.insert(charcharMapElement(0x2CBE, 0x2CBF)); // COPTIC CAPITAL LETTER OLD COPTIC OOU
  m.insert(charcharMapElement(0x2CC0, 0x2CC1)); // COPTIC CAPITAL LETTER SAMPI
  m.insert(charcharMapElement(0x2CC2, 0x2CC3)); // COPTIC CAPITAL LETTER CROSSED SHEI
  m.insert(charcharMapElement(0x2CC4, 0x2CC5)); // COPTIC CAPITAL LETTER OLD COPTIC SHEI
  m.insert(charcharMapElement(0x2CC6, 0x2CC7)); // COPTIC CAPITAL LETTER OLD COPTIC ESH
  m.insert(charcharMapElement(0x2CC8, 0x2CC9)); // COPTIC CAPITAL LETTER AKHMIMIC KHEI
  m.insert(charcharMapElement(0x2CCA, 0x2CCB)); // COPTIC CAPITAL LETTER DIALECT-P HORI
  m.insert(charcharMapElement(0x2CCC, 0x2CCD)); // COPTIC CAPITAL LETTER OLD COPTIC HORI
  m.insert(charcharMapElement(0x2CCE, 0x2CCF)); // COPTIC CAPITAL LETTER OLD COPTIC HA
  m.insert(charcharMapElement(0x2CD0, 0x2CD1)); // COPTIC CAPITAL LETTER L-SHAPED HA
  m.insert(charcharMapElement(0x2CD2, 0x2CD3)); // COPTIC CAPITAL LETTER OLD COPTIC HEI
  m.insert(charcharMapElement(0x2CD4, 0x2CD5)); // COPTIC CAPITAL LETTER OLD COPTIC HAT
  m.insert(charcharMapElement(0x2CD6, 0x2CD7)); // COPTIC CAPITAL LETTER OLD COPTIC GANGIA
  m.insert(charcharMapElement(0x2CD8, 0x2CD9)); // COPTIC CAPITAL LETTER OLD COPTIC DJA
  m.insert(charcharMapElement(0x2CDA, 0x2CDB)); // COPTIC CAPITAL LETTER OLD COPTIC SHIMA
  m.insert(charcharMapElement(0x2CDC, 0x2CDD)); // COPTIC CAPITAL LETTER OLD NUBIAN SHIMA
  m.insert(charcharMapElement(0x2CDE, 0x2CDF)); // COPTIC CAPITAL LETTER OLD NUBIAN NGI
  m.insert(charcharMapElement(0x2CE0, 0x2CE1)); // COPTIC CAPITAL LETTER OLD NUBIAN NYI
  m.insert(charcharMapElement(0x2CE2, 0x2CE3)); // COPTIC CAPITAL LETTER OLD NUBIAN WAU
  m.insert(charcharMapElement(0x2CEB, 0x2CEC)); // COPTIC CAPITAL LETTER CRYPTOGRAMMIC SHEI
  m.insert(charcharMapElement(0x2CED, 0x2CEE)); // COPTIC CAPITAL LETTER CRYPTOGRAMMIC GANGIA
  m.insert(charcharMapElement(0x2CF2, 0x2CF3)); // COPTIC CAPITAL LETTER BOHAIRIC KHEI
  m.insert(charcharMapElement(0xA640, 0xA641)); // CYRILLIC CAPITAL LETTER ZEMLYA
  m.insert(charcharMapElement(0xA642, 0xA643)); // CYRILLIC CAPITAL LETTER DZELO
  m.insert(charcharMapElement(0xA644, 0xA645)); // CYRILLIC CAPITAL LETTER REVERSED DZE
  m.insert(charcharMapElement(0xA646, 0xA647)); // CYRILLIC CAPITAL LETTER IOTA
  m.insert(charcharMapElement(0xA648, 0xA649)); // CYRILLIC CAPITAL LETTER DJERV
  m.insert(charcharMapElement(0xA64A, 0xA64B)); // CYRILLIC CAPITAL LETTER MONOGRAPH UK
  m.insert(charcharMapElement(0xA64C, 0xA64D)); // CYRILLIC CAPITAL LETTER BROAD OMEGA
  m.insert(charcharMapElement(0xA64E, 0xA64F)); // CYRILLIC CAPITAL LETTER NEUTRAL YER
  m.insert(charcharMapElement(0xA650, 0xA651)); // CYRILLIC CAPITAL LETTER YERU WITH BACK YER
  m.insert(charcharMapElement(0xA652, 0xA653)); // CYRILLIC CAPITAL LETTER IOTIFIED YAT
  m.insert(charcharMapElement(0xA654, 0xA655)); // CYRILLIC CAPITAL LETTER REVERSED YU
  m.insert(charcharMapElement(0xA656, 0xA657)); // CYRILLIC CAPITAL LETTER IOTIFIED A
  m.insert(charcharMapElement(0xA658, 0xA659)); // CYRILLIC CAPITAL LETTER CLOSED LITTLE YUS
  m.insert(charcharMapElement(0xA65A, 0xA65B)); // CYRILLIC CAPITAL LETTER BLENDED YUS
  m.insert(charcharMapElement(0xA65C, 0xA65D)); // CYRILLIC CAPITAL LETTER IOTIFIED CLOSED LITTLE YUS
  m.insert(charcharMapElement(0xA65E, 0xA65F)); // CYRILLIC CAPITAL LETTER YN
  m.insert(charcharMapElement(0xA660, 0xA661)); // CYRILLIC CAPITAL LETTER REVERSED TSE
  m.insert(charcharMapElement(0xA662, 0xA663)); // CYRILLIC CAPITAL LETTER SOFT DE
  m.insert(charcharMapElement(0xA664, 0xA665)); // CYRILLIC CAPITAL LETTER SOFT EL
  m.insert(charcharMapElement(0xA666, 0xA667)); // CYRILLIC CAPITAL LETTER SOFT EM
  m.insert(charcharMapElement(0xA668, 0xA669)); // CYRILLIC CAPITAL LETTER MONOCULAR O
  m.insert(charcharMapElement(0xA66A, 0xA66B)); // CYRILLIC CAPITAL LETTER BINOCULAR O
  m.insert(charcharMapElement(0xA66C, 0xA66D)); // CYRILLIC CAPITAL LETTER DOUBLE MONOCULAR O
  m.insert(charcharMapElement(0xA680, 0xA681)); // CYRILLIC CAPITAL LETTER DWE
  m.insert(charcharMapElement(0xA682, 0xA683)); // CYRILLIC CAPITAL LETTER DZWE
  m.insert(charcharMapElement(0xA684, 0xA685)); // CYRILLIC CAPITAL LETTER ZHWE
  m.insert(charcharMapElement(0xA686, 0xA687)); // CYRILLIC CAPITAL LETTER CCHE
  m.insert(charcharMapElement(0xA688, 0xA689)); // CYRILLIC CAPITAL LETTER DZZE
  m.insert(charcharMapElement(0xA68A, 0xA68B)); // CYRILLIC CAPITAL LETTER TE WITH MIDDLE HOOK
  m.insert(charcharMapElement(0xA68C, 0xA68D)); // CYRILLIC CAPITAL LETTER TWE
  m.insert(charcharMapElement(0xA68E, 0xA68F)); // CYRILLIC CAPITAL LETTER TSWE
  m.insert(charcharMapElement(0xA690, 0xA691)); // CYRILLIC CAPITAL LETTER TSSE
  m.insert(charcharMapElement(0xA692, 0xA693)); // CYRILLIC CAPITAL LETTER TCHE
  m.insert(charcharMapElement(0xA694, 0xA695)); // CYRILLIC CAPITAL LETTER HWE
  m.insert(charcharMapElement(0xA696, 0xA697)); // CYRILLIC CAPITAL LETTER SHWE
  m.insert(charcharMapElement(0xA722, 0xA723)); // LATIN CAPITAL LETTER EGYPTOLOGICAL ALEF
  m.insert(charcharMapElement(0xA724, 0xA725)); // LATIN CAPITAL LETTER EGYPTOLOGICAL AIN
  m.insert(charcharMapElement(0xA726, 0xA727)); // LATIN CAPITAL LETTER HENG
  m.insert(charcharMapElement(0xA728, 0xA729)); // LATIN CAPITAL LETTER TZ
  m.insert(charcharMapElement(0xA72A, 0xA72B)); // LATIN CAPITAL LETTER TRESILLO
  m.insert(charcharMapElement(0xA72C, 0xA72D)); // LATIN CAPITAL LETTER CUATRILLO
  m.insert(charcharMapElement(0xA72E, 0xA72F)); // LATIN CAPITAL LETTER CUATRILLO WITH COMMA
  m.insert(charcharMapElement(0xA732, 0xA733)); // LATIN CAPITAL LETTER AA
  m.insert(charcharMapElement(0xA734, 0xA735)); // LATIN CAPITAL LETTER AO
  m.insert(charcharMapElement(0xA736, 0xA737)); // LATIN CAPITAL LETTER AU
  m.insert(charcharMapElement(0xA738, 0xA739)); // LATIN CAPITAL LETTER AV
  m.insert(charcharMapElement(0xA73A, 0xA73B)); // LATIN CAPITAL LETTER AV WITH HORIZONTAL BAR
  m.insert(charcharMapElement(0xA73C, 0xA73D)); // LATIN CAPITAL LETTER AY
  m.insert(charcharMapElement(0xA73E, 0xA73F)); // LATIN CAPITAL LETTER REVERSED C WITH DOT
  m.insert(charcharMapElement(0xA740, 0xA741)); // LATIN CAPITAL LETTER K WITH STROKE
  m.insert(charcharMapElement(0xA742, 0xA743)); // LATIN CAPITAL LETTER K WITH DIAGONAL STROKE
  m.insert(charcharMapElement(0xA744, 0xA745)); // LATIN CAPITAL LETTER K WITH STROKE AND DIAGONAL STROKE
  m.insert(charcharMapElement(0xA746, 0xA747)); // LATIN CAPITAL LETTER BROKEN L
  m.insert(charcharMapElement(0xA748, 0xA749)); // LATIN CAPITAL LETTER L WITH HIGH STROKE
  m.insert(charcharMapElement(0xA74A, 0xA74B)); // LATIN CAPITAL LETTER O WITH LONG STROKE OVERLAY
  m.insert(charcharMapElement(0xA74C, 0xA74D)); // LATIN CAPITAL LETTER O WITH LOOP
  m.insert(charcharMapElement(0xA74E, 0xA74F)); // LATIN CAPITAL LETTER OO
  m.insert(charcharMapElement(0xA750, 0xA751)); // LATIN CAPITAL LETTER P WITH STROKE THROUGH DESCENDER
  m.insert(charcharMapElement(0xA752, 0xA753)); // LATIN CAPITAL LETTER P WITH FLOURISH
  m.insert(charcharMapElement(0xA754, 0xA755)); // LATIN CAPITAL LETTER P WITH SQUIRREL TAIL
  m.insert(charcharMapElement(0xA756, 0xA757)); // LATIN CAPITAL LETTER Q WITH STROKE THROUGH DESCENDER
  m.insert(charcharMapElement(0xA758, 0xA759)); // LATIN CAPITAL LETTER Q WITH DIAGONAL STROKE
  m.insert(charcharMapElement(0xA75A, 0xA75B)); // LATIN CAPITAL LETTER R ROTUNDA
  m.insert(charcharMapElement(0xA75C, 0xA75D)); // LATIN CAPITAL LETTER RUM ROTUNDA
  m.insert(charcharMapElement(0xA75E, 0xA75F)); // LATIN CAPITAL LETTER V WITH DIAGONAL STROKE
  m.insert(charcharMapElement(0xA760, 0xA761)); // LATIN CAPITAL LETTER VY
  m.insert(charcharMapElement(0xA762, 0xA763)); // LATIN CAPITAL LETTER VISIGOTHIC Z
  m.insert(charcharMapElement(0xA764, 0xA765)); // LATIN CAPITAL LETTER THORN WITH STROKE
  m.insert(charcharMapElement(0xA766, 0xA767)); // LATIN CAPITAL LETTER THORN WITH STROKE THROUGH DESCENDER
  m.insert(charcharMapElement(0xA768, 0xA769)); // LATIN CAPITAL LETTER VEND
  m.insert(charcharMapElement(0xA76A, 0xA76B)); // LATIN CAPITAL LETTER ET
  m.insert(charcharMapElement(0xA76C, 0xA76D)); // LATIN CAPITAL LETTER IS
  m.insert(charcharMapElement(0xA76E, 0xA76F)); // LATIN CAPITAL LETTER CON
  m.insert(charcharMapElement(0xA779, 0xA77A)); // LATIN CAPITAL LETTER INSULAR D
  m.insert(charcharMapElement(0xA77B, 0xA77C)); // LATIN CAPITAL LETTER INSULAR F
  m.insert(charcharMapElement(0xA77D, 0x1D79)); // LATIN CAPITAL LETTER INSULAR G
  m.insert(charcharMapElement(0xA77E, 0xA77F)); // LATIN CAPITAL LETTER TURNED INSULAR G
  m.insert(charcharMapElement(0xA780, 0xA781)); // LATIN CAPITAL LETTER TURNED L
  m.insert(charcharMapElement(0xA782, 0xA783)); // LATIN CAPITAL LETTER INSULAR R
  m.insert(charcharMapElement(0xA784, 0xA785)); // LATIN CAPITAL LETTER INSULAR S
  m.insert(charcharMapElement(0xA786, 0xA787)); // LATIN CAPITAL LETTER INSULAR T
  m.insert(charcharMapElement(0xA78B, 0xA78C)); // LATIN CAPITAL LETTER SALTILLO
  m.insert(charcharMapElement(0xA78D, 0x0265)); // LATIN CAPITAL LETTER TURNED H
  m.insert(charcharMapElement(0xA790, 0xA791)); // LATIN CAPITAL LETTER N WITH DESCENDER
  m.insert(charcharMapElement(0xA792, 0xA793)); // LATIN CAPITAL LETTER C WITH BAR
  m.insert(charcharMapElement(0xA7A0, 0xA7A1)); // LATIN CAPITAL LETTER G WITH OBLIQUE STROKE
  m.insert(charcharMapElement(0xA7A2, 0xA7A3)); // LATIN CAPITAL LETTER K WITH OBLIQUE STROKE
  m.insert(charcharMapElement(0xA7A4, 0xA7A5)); // LATIN CAPITAL LETTER N WITH OBLIQUE STROKE
  m.insert(charcharMapElement(0xA7A6, 0xA7A7)); // LATIN CAPITAL LETTER R WITH OBLIQUE STROKE
  m.insert(charcharMapElement(0xA7A8, 0xA7A9)); // LATIN CAPITAL LETTER S WITH OBLIQUE STROKE
  m.insert(charcharMapElement(0xA7AA, 0x0266)); // LATIN CAPITAL LETTER H WITH HOOK
  m.insert(charcharMapElement(0xFF21, 0xFF41)); // FULLWIDTH LATIN CAPITAL LETTER A
  m.insert(charcharMapElement(0xFF22, 0xFF42)); // FULLWIDTH LATIN CAPITAL LETTER B
  m.insert(charcharMapElement(0xFF23, 0xFF43)); // FULLWIDTH LATIN CAPITAL LETTER C
  m.insert(charcharMapElement(0xFF24, 0xFF44)); // FULLWIDTH LATIN CAPITAL LETTER D
  m.insert(charcharMapElement(0xFF25, 0xFF45)); // FULLWIDTH LATIN CAPITAL LETTER E
  m.insert(charcharMapElement(0xFF26, 0xFF46)); // FULLWIDTH LATIN CAPITAL LETTER F
  m.insert(charcharMapElement(0xFF27, 0xFF47)); // FULLWIDTH LATIN CAPITAL LETTER G
  m.insert(charcharMapElement(0xFF28, 0xFF48)); // FULLWIDTH LATIN CAPITAL LETTER H
  m.insert(charcharMapElement(0xFF29, 0xFF49)); // FULLWIDTH LATIN CAPITAL LETTER I
  m.insert(charcharMapElement(0xFF2A, 0xFF4A)); // FULLWIDTH LATIN CAPITAL LETTER J
  m.insert(charcharMapElement(0xFF2B, 0xFF4B)); // FULLWIDTH LATIN CAPITAL LETTER K
  m.insert(charcharMapElement(0xFF2C, 0xFF4C)); // FULLWIDTH LATIN CAPITAL LETTER L
  m.insert(charcharMapElement(0xFF2D, 0xFF4D)); // FULLWIDTH LATIN CAPITAL LETTER M
  m.insert(charcharMapElement(0xFF2E, 0xFF4E)); // FULLWIDTH LATIN CAPITAL LETTER N
  m.insert(charcharMapElement(0xFF2F, 0xFF4F)); // FULLWIDTH LATIN CAPITAL LETTER O
  m.insert(charcharMapElement(0xFF30, 0xFF50)); // FULLWIDTH LATIN CAPITAL LETTER P
  m.insert(charcharMapElement(0xFF31, 0xFF51)); // FULLWIDTH LATIN CAPITAL LETTER Q
  m.insert(charcharMapElement(0xFF32, 0xFF52)); // FULLWIDTH LATIN CAPITAL LETTER R
  m.insert(charcharMapElement(0xFF33, 0xFF53)); // FULLWIDTH LATIN CAPITAL LETTER S
  m.insert(charcharMapElement(0xFF34, 0xFF54)); // FULLWIDTH LATIN CAPITAL LETTER T
  m.insert(charcharMapElement(0xFF35, 0xFF55)); // FULLWIDTH LATIN CAPITAL LETTER U
  m.insert(charcharMapElement(0xFF36, 0xFF56)); // FULLWIDTH LATIN CAPITAL LETTER V
  m.insert(charcharMapElement(0xFF37, 0xFF57)); // FULLWIDTH LATIN CAPITAL LETTER W
  m.insert(charcharMapElement(0xFF38, 0xFF58)); // FULLWIDTH LATIN CAPITAL LETTER X
  m.insert(charcharMapElement(0xFF39, 0xFF59)); // FULLWIDTH LATIN CAPITAL LETTER Y
  m.insert(charcharMapElement(0xFF3A, 0xFF5A)); // FULLWIDTH LATIN CAPITAL LETTER Z
  m.insert(charcharMapElement(0x10400, 0x10428)); // DESERET CAPITAL LETTER LONG I
  m.insert(charcharMapElement(0x10401, 0x10429)); // DESERET CAPITAL LETTER LONG E
  m.insert(charcharMapElement(0x10402, 0x1042A)); // DESERET CAPITAL LETTER LONG A
  m.insert(charcharMapElement(0x10403, 0x1042B)); // DESERET CAPITAL LETTER LONG AH
  m.insert(charcharMapElement(0x10404, 0x1042C)); // DESERET CAPITAL LETTER LONG O
  m.insert(charcharMapElement(0x10405, 0x1042D)); // DESERET CAPITAL LETTER LONG OO
  m.insert(charcharMapElement(0x10406, 0x1042E)); // DESERET CAPITAL LETTER SHORT I
  m.insert(charcharMapElement(0x10407, 0x1042F)); // DESERET CAPITAL LETTER SHORT E
  m.insert(charcharMapElement(0x10408, 0x10430)); // DESERET CAPITAL LETTER SHORT A
  m.insert(charcharMapElement(0x10409, 0x10431)); // DESERET CAPITAL LETTER SHORT AH
  m.insert(charcharMapElement(0x1040A, 0x10432)); // DESERET CAPITAL LETTER SHORT O
  m.insert(charcharMapElement(0x1040B, 0x10433)); // DESERET CAPITAL LETTER SHORT OO
  m.insert(charcharMapElement(0x1040C, 0x10434)); // DESERET CAPITAL LETTER AY
  m.insert(charcharMapElement(0x1040D, 0x10435)); // DESERET CAPITAL LETTER OW
  m.insert(charcharMapElement(0x1040E, 0x10436)); // DESERET CAPITAL LETTER WU
  m.insert(charcharMapElement(0x1040F, 0x10437)); // DESERET CAPITAL LETTER YEE
  m.insert(charcharMapElement(0x10410, 0x10438)); // DESERET CAPITAL LETTER H
  m.insert(charcharMapElement(0x10411, 0x10439)); // DESERET CAPITAL LETTER PEE
  m.insert(charcharMapElement(0x10412, 0x1043A)); // DESERET CAPITAL LETTER BEE
  m.insert(charcharMapElement(0x10413, 0x1043B)); // DESERET CAPITAL LETTER TEE
  m.insert(charcharMapElement(0x10414, 0x1043C)); // DESERET CAPITAL LETTER DEE
  m.insert(charcharMapElement(0x10415, 0x1043D)); // DESERET CAPITAL LETTER CHEE
  m.insert(charcharMapElement(0x10416, 0x1043E)); // DESERET CAPITAL LETTER JEE
  m.insert(charcharMapElement(0x10417, 0x1043F)); // DESERET CAPITAL LETTER KAY
  m.insert(charcharMapElement(0x10418, 0x10440)); // DESERET CAPITAL LETTER GAY
  m.insert(charcharMapElement(0x10419, 0x10441)); // DESERET CAPITAL LETTER EF
  m.insert(charcharMapElement(0x1041A, 0x10442)); // DESERET CAPITAL LETTER VEE
  m.insert(charcharMapElement(0x1041B, 0x10443)); // DESERET CAPITAL LETTER ETH
  m.insert(charcharMapElement(0x1041C, 0x10444)); // DESERET CAPITAL LETTER THEE
  m.insert(charcharMapElement(0x1041D, 0x10445)); // DESERET CAPITAL LETTER ES
  m.insert(charcharMapElement(0x1041E, 0x10446)); // DESERET CAPITAL LETTER ZEE
  m.insert(charcharMapElement(0x1041F, 0x10447)); // DESERET CAPITAL LETTER ESH
  m.insert(charcharMapElement(0x10420, 0x10448)); // DESERET CAPITAL LETTER ZHEE
  m.insert(charcharMapElement(0x10421, 0x10449)); // DESERET CAPITAL LETTER ER
  m.insert(charcharMapElement(0x10422, 0x1044A)); // DESERET CAPITAL LETTER EL
  m.insert(charcharMapElement(0x10423, 0x1044B)); // DESERET CAPITAL LETTER EM
  m.insert(charcharMapElement(0x10424, 0x1044C)); // DESERET CAPITAL LETTER EN
  m.insert(charcharMapElement(0x10425, 0x1044D)); // DESERET CAPITAL LETTER ENG
  m.insert(charcharMapElement(0x10426, 0x1044E)); // DESERET CAPITAL LETTER OI
  m.insert(charcharMapElement(0x10427, 0x1044F)); // DESERET CAPITAL LETTER EW

  return m;
}

CUtf32Utils::charstrMap CUtf32Utils::foldFullCharsMapFiller(void)
{
  const struct mapFillElement
  {
    charstrMap::key_type Key;
    charstrMap::mapped_type Value;
  } mapFiller[] =
  {
    { 0x00DF, { { 0x0073, 0x0073 }, 2 } }, // LATIN SMALL LETTER SHARP S
    { 0x0130, { { 0x0069, 0x0307 }, 2 } }, // LATIN CAPITAL LETTER I WITH DOT ABOVE
    { 0x0149, { { 0x02BC, 0x006E }, 2 } }, // LATIN SMALL LETTER N PRECEDED BY APOSTROPHE
    { 0x01F0, { { 0x006A, 0x030C }, 2 } }, // LATIN SMALL LETTER J WITH CARON
    { 0x0390, { { 0x03B9, 0x0308, 0x0301 }, 3 } }, // GREEK SMALL LETTER IOTA WITH DIALYTIKA AND TONOS
    { 0x03B0, { { 0x03C5, 0x0308, 0x0301 }, 3 } }, // GREEK SMALL LETTER UPSILON WITH DIALYTIKA AND TONOS
    { 0x0587, { { 0x0565, 0x0582 }, 2 } }, // ARMENIAN SMALL LIGATURE ECH YIWN
    { 0x1E96, { { 0x0068, 0x0331 }, 2 } }, // LATIN SMALL LETTER H WITH LINE BELOW
    { 0x1E97, { { 0x0074, 0x0308 }, 2 } }, // LATIN SMALL LETTER T WITH DIAERESIS
    { 0x1E98, { { 0x0077, 0x030A }, 2 } }, // LATIN SMALL LETTER W WITH RING ABOVE
    { 0x1E99, { { 0x0079, 0x030A }, 2 } }, // LATIN SMALL LETTER Y WITH RING ABOVE
    { 0x1E9A, { { 0x0061, 0x02BE }, 2 } }, // LATIN SMALL LETTER A WITH RIGHT HALF RING
    { 0x1E9E, { { 0x0073, 0x0073 }, 2 } }, // LATIN CAPITAL LETTER SHARP S
    { 0x1F50, { { 0x03C5, 0x0313 }, 2 } }, // GREEK SMALL LETTER UPSILON WITH PSILI
    { 0x1F52, { { 0x03C5, 0x0313, 0x0300 }, 3 } }, // GREEK SMALL LETTER UPSILON WITH PSILI AND VARIA
    { 0x1F54, { { 0x03C5, 0x0313, 0x0301 }, 3 } }, // GREEK SMALL LETTER UPSILON WITH PSILI AND OXIA
    { 0x1F56, { { 0x03C5, 0x0313, 0x0342 }, 3 } }, // GREEK SMALL LETTER UPSILON WITH PSILI AND PERISPOMENI
    { 0x1F80, { { 0x1F00, 0x03B9 }, 2 } }, // GREEK SMALL LETTER ALPHA WITH PSILI AND YPOGEGRAMMENI
    { 0x1F81, { { 0x1F01, 0x03B9 }, 2 } }, // GREEK SMALL LETTER ALPHA WITH DASIA AND YPOGEGRAMMENI
    { 0x1F82, { { 0x1F02, 0x03B9 }, 2 } }, // GREEK SMALL LETTER ALPHA WITH PSILI AND VARIA AND YPOGEGRAMMENI
    { 0x1F83, { { 0x1F03, 0x03B9 }, 2 } }, // GREEK SMALL LETTER ALPHA WITH DASIA AND VARIA AND YPOGEGRAMMENI
    { 0x1F84, { { 0x1F04, 0x03B9 }, 2 } }, // GREEK SMALL LETTER ALPHA WITH PSILI AND OXIA AND YPOGEGRAMMENI
    { 0x1F85, { { 0x1F05, 0x03B9 }, 2 } }, // GREEK SMALL LETTER ALPHA WITH DASIA AND OXIA AND YPOGEGRAMMENI
    { 0x1F86, { { 0x1F06, 0x03B9 }, 2 } }, // GREEK SMALL LETTER ALPHA WITH PSILI AND PERISPOMENI AND YPOGEGRAMMENI
    { 0x1F87, { { 0x1F07, 0x03B9 }, 2 } }, // GREEK SMALL LETTER ALPHA WITH DASIA AND PERISPOMENI AND YPOGEGRAMMENI
    { 0x1F88, { { 0x1F00, 0x03B9 }, 2 } }, // GREEK CAPITAL LETTER ALPHA WITH PSILI AND PROSGEGRAMMENI
    { 0x1F89, { { 0x1F01, 0x03B9 }, 2 } }, // GREEK CAPITAL LETTER ALPHA WITH DASIA AND PROSGEGRAMMENI
    { 0x1F8A, { { 0x1F02, 0x03B9 }, 2 } }, // GREEK CAPITAL LETTER ALPHA WITH PSILI AND VARIA AND PROSGEGRAMMENI
    { 0x1F8B, { { 0x1F03, 0x03B9 }, 2 } }, // GREEK CAPITAL LETTER ALPHA WITH DASIA AND VARIA AND PROSGEGRAMMENI
    { 0x1F8C, { { 0x1F04, 0x03B9 }, 2 } }, // GREEK CAPITAL LETTER ALPHA WITH PSILI AND OXIA AND PROSGEGRAMMENI
    { 0x1F8D, { { 0x1F05, 0x03B9 }, 2 } }, // GREEK CAPITAL LETTER ALPHA WITH DASIA AND OXIA AND PROSGEGRAMMENI
    { 0x1F8E, { { 0x1F06, 0x03B9 }, 2 } }, // GREEK CAPITAL LETTER ALPHA WITH PSILI AND PERISPOMENI AND PROSGEGRAMMENI
    { 0x1F8F, { { 0x1F07, 0x03B9 }, 2 } }, // GREEK CAPITAL LETTER ALPHA WITH DASIA AND PERISPOMENI AND PROSGEGRAMMENI
    { 0x1F90, { { 0x1F20, 0x03B9 }, 2 } }, // GREEK SMALL LETTER ETA WITH PSILI AND YPOGEGRAMMENI
    { 0x1F91, { { 0x1F21, 0x03B9 }, 2 } }, // GREEK SMALL LETTER ETA WITH DASIA AND YPOGEGRAMMENI
    { 0x1F92, { { 0x1F22, 0x03B9 }, 2 } }, // GREEK SMALL LETTER ETA WITH PSILI AND VARIA AND YPOGEGRAMMENI
    { 0x1F93, { { 0x1F23, 0x03B9 }, 2 } }, // GREEK SMALL LETTER ETA WITH DASIA AND VARIA AND YPOGEGRAMMENI
    { 0x1F94, { { 0x1F24, 0x03B9 }, 2 } }, // GREEK SMALL LETTER ETA WITH PSILI AND OXIA AND YPOGEGRAMMENI
    { 0x1F95, { { 0x1F25, 0x03B9 }, 2 } }, // GREEK SMALL LETTER ETA WITH DASIA AND OXIA AND YPOGEGRAMMENI
    { 0x1F96, { { 0x1F26, 0x03B9 }, 2 } }, // GREEK SMALL LETTER ETA WITH PSILI AND PERISPOMENI AND YPOGEGRAMMENI
    { 0x1F97, { { 0x1F27, 0x03B9 }, 2 } }, // GREEK SMALL LETTER ETA WITH DASIA AND PERISPOMENI AND YPOGEGRAMMENI
    { 0x1F98, { { 0x1F20, 0x03B9 }, 2 } }, // GREEK CAPITAL LETTER ETA WITH PSILI AND PROSGEGRAMMENI
    { 0x1F99, { { 0x1F21, 0x03B9 }, 2 } }, // GREEK CAPITAL LETTER ETA WITH DASIA AND PROSGEGRAMMENI
    { 0x1F9A, { { 0x1F22, 0x03B9 }, 2 } }, // GREEK CAPITAL LETTER ETA WITH PSILI AND VARIA AND PROSGEGRAMMENI
    { 0x1F9B, { { 0x1F23, 0x03B9 }, 2 } }, // GREEK CAPITAL LETTER ETA WITH DASIA AND VARIA AND PROSGEGRAMMENI
    { 0x1F9C, { { 0x1F24, 0x03B9 }, 2 } }, // GREEK CAPITAL LETTER ETA WITH PSILI AND OXIA AND PROSGEGRAMMENI
    { 0x1F9D, { { 0x1F25, 0x03B9 }, 2 } }, // GREEK CAPITAL LETTER ETA WITH DASIA AND OXIA AND PROSGEGRAMMENI
    { 0x1F9E, { { 0x1F26, 0x03B9 }, 2 } }, // GREEK CAPITAL LETTER ETA WITH PSILI AND PERISPOMENI AND PROSGEGRAMMENI
    { 0x1F9F, { { 0x1F27, 0x03B9 }, 2 } }, // GREEK CAPITAL LETTER ETA WITH DASIA AND PERISPOMENI AND PROSGEGRAMMENI
    { 0x1FA0, { { 0x1F60, 0x03B9 }, 2 } }, // GREEK SMALL LETTER OMEGA WITH PSILI AND YPOGEGRAMMENI
    { 0x1FA1, { { 0x1F61, 0x03B9 }, 2 } }, // GREEK SMALL LETTER OMEGA WITH DASIA AND YPOGEGRAMMENI
    { 0x1FA2, { { 0x1F62, 0x03B9 }, 2 } }, // GREEK SMALL LETTER OMEGA WITH PSILI AND VARIA AND YPOGEGRAMMENI
    { 0x1FA3, { { 0x1F63, 0x03B9 }, 2 } }, // GREEK SMALL LETTER OMEGA WITH DASIA AND VARIA AND YPOGEGRAMMENI
    { 0x1FA4, { { 0x1F64, 0x03B9 }, 2 } }, // GREEK SMALL LETTER OMEGA WITH PSILI AND OXIA AND YPOGEGRAMMENI
    { 0x1FA5, { { 0x1F65, 0x03B9 }, 2 } }, // GREEK SMALL LETTER OMEGA WITH DASIA AND OXIA AND YPOGEGRAMMENI
    { 0x1FA6, { { 0x1F66, 0x03B9 }, 2 } }, // GREEK SMALL LETTER OMEGA WITH PSILI AND PERISPOMENI AND YPOGEGRAMMENI
    { 0x1FA7, { { 0x1F67, 0x03B9 }, 2 } }, // GREEK SMALL LETTER OMEGA WITH DASIA AND PERISPOMENI AND YPOGEGRAMMENI
    { 0x1FA8, { { 0x1F60, 0x03B9 }, 2 } }, // GREEK CAPITAL LETTER OMEGA WITH PSILI AND PROSGEGRAMMENI
    { 0x1FA9, { { 0x1F61, 0x03B9 }, 2 } }, // GREEK CAPITAL LETTER OMEGA WITH DASIA AND PROSGEGRAMMENI
    { 0x1FAA, { { 0x1F62, 0x03B9 }, 2 } }, // GREEK CAPITAL LETTER OMEGA WITH PSILI AND VARIA AND PROSGEGRAMMENI
    { 0x1FAB, { { 0x1F63, 0x03B9 }, 2 } }, // GREEK CAPITAL LETTER OMEGA WITH DASIA AND VARIA AND PROSGEGRAMMENI
    { 0x1FAC, { { 0x1F64, 0x03B9 }, 2 } }, // GREEK CAPITAL LETTER OMEGA WITH PSILI AND OXIA AND PROSGEGRAMMENI
    { 0x1FAD, { { 0x1F65, 0x03B9 }, 2 } }, // GREEK CAPITAL LETTER OMEGA WITH DASIA AND OXIA AND PROSGEGRAMMENI
    { 0x1FAE, { { 0x1F66, 0x03B9 }, 2 } }, // GREEK CAPITAL LETTER OMEGA WITH PSILI AND PERISPOMENI AND PROSGEGRAMMENI
    { 0x1FAF, { { 0x1F67, 0x03B9 }, 2 } }, // GREEK CAPITAL LETTER OMEGA WITH DASIA AND PERISPOMENI AND PROSGEGRAMMENI
    { 0x1FB2, { { 0x1F70, 0x03B9 }, 2 } }, // GREEK SMALL LETTER ALPHA WITH VARIA AND YPOGEGRAMMENI
    { 0x1FB3, { { 0x03B1, 0x03B9 }, 2 } }, // GREEK SMALL LETTER ALPHA WITH YPOGEGRAMMENI
    { 0x1FB4, { { 0x03AC, 0x03B9 }, 2 } }, // GREEK SMALL LETTER ALPHA WITH OXIA AND YPOGEGRAMMENI
    { 0x1FB6, { { 0x03B1, 0x0342 }, 2 } }, // GREEK SMALL LETTER ALPHA WITH PERISPOMENI
    { 0x1FB7, { { 0x03B1, 0x0342, 0x03B9 }, 3 } }, // GREEK SMALL LETTER ALPHA WITH PERISPOMENI AND YPOGEGRAMMENI
    { 0x1FBC, { { 0x03B1, 0x03B9 }, 2 } }, // GREEK CAPITAL LETTER ALPHA WITH PROSGEGRAMMENI
    { 0x1FC2, { { 0x1F74, 0x03B9 }, 2 } }, // GREEK SMALL LETTER ETA WITH VARIA AND YPOGEGRAMMENI
    { 0x1FC3, { { 0x03B7, 0x03B9 }, 2 } }, // GREEK SMALL LETTER ETA WITH YPOGEGRAMMENI
    { 0x1FC4, { { 0x03AE, 0x03B9 }, 2 } }, // GREEK SMALL LETTER ETA WITH OXIA AND YPOGEGRAMMENI
    { 0x1FC6, { { 0x03B7, 0x0342 }, 2 } }, // GREEK SMALL LETTER ETA WITH PERISPOMENI
    { 0x1FC7, { { 0x03B7, 0x0342, 0x03B9 }, 3 } }, // GREEK SMALL LETTER ETA WITH PERISPOMENI AND YPOGEGRAMMENI
    { 0x1FCC, { { 0x03B7, 0x03B9 }, 2 } }, // GREEK CAPITAL LETTER ETA WITH PROSGEGRAMMENI
    { 0x1FD2, { { 0x03B9, 0x0308, 0x0300 }, 3 } }, // GREEK SMALL LETTER IOTA WITH DIALYTIKA AND VARIA
    { 0x1FD3, { { 0x03B9, 0x0308, 0x0301 }, 3 } }, // GREEK SMALL LETTER IOTA WITH DIALYTIKA AND OXIA
    { 0x1FD6, { { 0x03B9, 0x0342 }, 2 } }, // GREEK SMALL LETTER IOTA WITH PERISPOMENI
    { 0x1FD7, { { 0x03B9, 0x0308, 0x0342 }, 3 } }, // GREEK SMALL LETTER IOTA WITH DIALYTIKA AND PERISPOMENI
    { 0x1FE2, { { 0x03C5, 0x0308, 0x0300 }, 3 } }, // GREEK SMALL LETTER UPSILON WITH DIALYTIKA AND VARIA
    { 0x1FE3, { { 0x03C5, 0x0308, 0x0301 }, 3 } }, // GREEK SMALL LETTER UPSILON WITH DIALYTIKA AND OXIA
    { 0x1FE4, { { 0x03C1, 0x0313 }, 2 } }, // GREEK SMALL LETTER RHO WITH PSILI
    { 0x1FE6, { { 0x03C5, 0x0342 }, 2 } }, // GREEK SMALL LETTER UPSILON WITH PERISPOMENI
    { 0x1FE7, { { 0x03C5, 0x0308, 0x0342 }, 3 } }, // GREEK SMALL LETTER UPSILON WITH DIALYTIKA AND PERISPOMENI
    { 0x1FF2, { { 0x1F7C, 0x03B9 }, 2 } }, // GREEK SMALL LETTER OMEGA WITH VARIA AND YPOGEGRAMMENI
    { 0x1FF3, { { 0x03C9, 0x03B9 }, 2 } }, // GREEK SMALL LETTER OMEGA WITH YPOGEGRAMMENI
    { 0x1FF4, { { 0x03CE, 0x03B9 }, 2 } }, // GREEK SMALL LETTER OMEGA WITH OXIA AND YPOGEGRAMMENI
    { 0x1FF6, { { 0x03C9, 0x0342 }, 2 } }, // GREEK SMALL LETTER OMEGA WITH PERISPOMENI
    { 0x1FF7, { { 0x03C9, 0x0342, 0x03B9 }, 3 } }, // GREEK SMALL LETTER OMEGA WITH PERISPOMENI AND YPOGEGRAMMENI
    { 0x1FFC, { { 0x03C9, 0x03B9 }, 2 } }, // GREEK CAPITAL LETTER OMEGA WITH PROSGEGRAMMENI
    { 0xFB00, { { 0x0066, 0x0066 }, 2 } }, // LATIN SMALL LIGATURE FF
    { 0xFB01, { { 0x0066, 0x0069 }, 2 } }, // LATIN SMALL LIGATURE FI
    { 0xFB02, { { 0x0066, 0x006C }, 2 } }, // LATIN SMALL LIGATURE FL
    { 0xFB03, { { 0x0066, 0x0066, 0x0069 }, 3 } }, // LATIN SMALL LIGATURE FFI
    { 0xFB04, { { 0x0066, 0x0066, 0x006C }, 3 } }, // LATIN SMALL LIGATURE FFL
    { 0xFB05, { { 0x0073, 0x0074 }, 2 } }, // LATIN SMALL LIGATURE LONG S T
    { 0xFB06, { { 0x0073, 0x0074 }, 2 } }, // LATIN SMALL LIGATURE ST
    { 0xFB13, { { 0x0574, 0x0576 }, 2 } }, // ARMENIAN SMALL LIGATURE MEN NOW
    { 0xFB14, { { 0x0574, 0x0565 }, 2 } }, // ARMENIAN SMALL LIGATURE MEN ECH
    { 0xFB15, { { 0x0574, 0x056B }, 2 } }, // ARMENIAN SMALL LIGATURE MEN INI
    { 0xFB16, { { 0x057E, 0x0576 }, 2 } }, // ARMENIAN SMALL LIGATURE VEW NOW
    { 0xFB17, { { 0x0574, 0x056D }, 2 } }, // ARMENIAN SMALL LIGATURE MEN XEH
    { 0, { { 0 }, 0 } }
  };

  charstrMap m;
  for (int i = 0; mapFiller[i].Key; i++)
    m.insert(charstrMapElement(mapFiller[i].Key, mapFiller[i].Value));

  return m;
}

