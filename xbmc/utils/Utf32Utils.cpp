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

#include "Utf32Utils.h"

const CUtf32Utils::digitsMap CUtf32Utils::m_digitsMap(CUtf32Utils::digitsMapFiller());

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


CUtf32Utils::digitsMap CUtf32Utils::digitsMapFiller(void)
{
  digitsMap m;

  // source: DerivedNumericValues-6.3.0.txt
  // see ftp://ftp.unicode.org/Public/UCD/latest/ucd/extracted/DerivedNumericValues.txt

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
