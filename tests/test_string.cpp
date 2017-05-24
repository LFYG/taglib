/***************************************************************************
    copyright           : (C) 2007 by Lukas Lalinsky
    email               : lukas@oxygene.sk
 ***************************************************************************/

/***************************************************************************
 *   This library is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU Lesser General Public License version   *
 *   2.1 as published by the Free Software Foundation.                     *
 *                                                                         *
 *   This library is distributed in the hope that it will be useful, but   *
 *   WITHOUT ANY WARRANTY; without even the implied warranty of            *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU     *
 *   Lesser General Public License for more details.                       *
 *                                                                         *
 *   You should have received a copy of the GNU Lesser General Public      *
 *   License along with this library; if not, write to the Free Software   *
 *   Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA         *
 *   02110-1301  USA                                                       *
 *                                                                         *
 *   Alternatively, this file is available under the Mozilla Public        *
 *   License Version 1.1.  You may obtain a copy of the License at         *
 *   http://www.mozilla.org/MPL/                                           *
 ***************************************************************************/

#include <tstring.h>
#include <boost/test/unit_test.hpp>
#include "loghelpers.h"

using namespace TagLib;

BOOST_AUTO_TEST_SUITE(TestString)

BOOST_AUTO_TEST_CASE(testString)
{
  String s = "taglib string";
  ByteVector v = "taglib string";
  BOOST_CHECK_EQUAL(s.data(String::Latin1), v);
  
  BOOST_CHECK(strcmp(s.toCString(), "taglib string") == 0);
  BOOST_CHECK_EQUAL(s, "taglib string");
  BOOST_CHECK_NE(s, "taglib STRING");
  BOOST_CHECK_NE(s, "taglib");
  BOOST_CHECK_NE(s, "taglib string taglib");
  BOOST_CHECK_EQUAL(s, L"taglib string");
  BOOST_CHECK_NE(s, L"taglib STRING");
  BOOST_CHECK_NE(s, L"taglib");
  BOOST_CHECK_NE(s, L"taglib string taglib");
  
  s.clear();
  BOOST_CHECK(s.isEmpty());
  BOOST_CHECK(!s.isNull()); // deprecated, but still worth it to check.
  
  String unicode("José Carlos", String::UTF8);
  BOOST_CHECK(strcmp(unicode.toCString(), "Jos\xe9 Carlos") == 0);
  
  String latin = "Jos\xe9 Carlos";
  BOOST_CHECK(strcmp(latin.toCString(true), "José Carlos") == 0);
  
  String c;
  c = "1";
  BOOST_CHECK_EQUAL(c, L"1");
  
  c = L'\u4E00';
  BOOST_CHECK_EQUAL(c, L"\u4E00");
  
  String unicode2(unicode.to8Bit(true), String::UTF8);
  BOOST_CHECK_EQUAL(unicode, unicode2);
  
  String unicode3(L"\u65E5\u672C\u8A9E");
  BOOST_CHECK_EQUAL(*(unicode3.toCWString() + 1), L'\u672C');
  
  String unicode4(L"\u65e5\u672c\u8a9e", String::UTF16BE);
  BOOST_CHECK_EQUAL(unicode4[1], L'\u672c');
  
  String unicode5(L"\u65e5\u672c\u8a9e", String::UTF16LE);
  BOOST_CHECK_EQUAL(unicode5[1], L'\u2c67');
  
  const std::wstring stduni = L"\u65e5\u672c\u8a9e";
  
  String unicode6(stduni, String::UTF16BE);
  BOOST_CHECK_EQUAL(unicode6[1], L'\u672c');
  
  String unicode7(stduni, String::UTF16LE);
  BOOST_CHECK_EQUAL(unicode7[1], L'\u2c67');
  
  BOOST_CHECK_EQUAL(String("  foo  ").stripWhiteSpace(), "foo");
  BOOST_CHECK_EQUAL(String("foo    ").stripWhiteSpace(), "foo");
  BOOST_CHECK_EQUAL(String("    foo").stripWhiteSpace(), "foo");
  BOOST_CHECK_EQUAL(String("foo").stripWhiteSpace(), "foo");
  BOOST_CHECK_EQUAL(String("f o o").stripWhiteSpace(), "f o o");
  BOOST_CHECK_EQUAL(String(" f o o ").stripWhiteSpace(), "f o o");
  
  BOOST_CHECK(memcmp(String("foo").data(String::Latin1).data(), "foo", 3) == 0);
  BOOST_CHECK(memcmp(String("f").data(String::Latin1).data(), "f", 1) == 0);
}

BOOST_AUTO_TEST_CASE(testRfind)
{
  BOOST_CHECK_EQUAL(String("foo.bar").rfind(".", 0), -1);
  BOOST_CHECK_EQUAL(String("foo.bar").rfind(".", 1), -1);
  BOOST_CHECK_EQUAL(String("foo.bar").rfind(".", 2), -1);
  BOOST_CHECK_EQUAL(String("foo.bar").rfind(".", 3), 3);
  BOOST_CHECK_EQUAL(String("foo.bar").rfind(".", 4), 3);
  BOOST_CHECK_EQUAL(String("foo.bar").rfind(".", 5), 3);
  BOOST_CHECK_EQUAL(String("foo.bar").rfind(".", 6), 3);
  BOOST_CHECK_EQUAL(String("foo.bar").rfind(".", 7), 3);
  BOOST_CHECK_EQUAL(String("foo.bar").rfind("."), 3);
}

BOOST_AUTO_TEST_CASE(testUTF16Encode)
{
  const String a("foo");
  const ByteVector b("\0f\0o\0o", 6);
  const ByteVector c("f\0o\0o\0", 6);
  const ByteVector d("\377\376f\0o\0o\0", 8);
  BOOST_CHECK_NE(a.data(String::UTF16BE), a.data(String::UTF16LE));
  BOOST_CHECK_EQUAL(a.data(String::UTF16BE), b);
  BOOST_CHECK_EQUAL(a.data(String::UTF16LE), c);
  BOOST_CHECK_EQUAL(a.data(String::UTF16), d);
}

BOOST_AUTO_TEST_CASE(testUTF16Decode)
{
  const String a("foo");
  const ByteVector b("\0f\0o\0o", 6);
  const ByteVector c("f\0o\0o\0", 6);
  const ByteVector d("\377\376f\0o\0o\0", 8);
  BOOST_CHECK_EQUAL(String(b, String::UTF16BE), a);
  BOOST_CHECK_EQUAL(String(c, String::UTF16LE), a);
  BOOST_CHECK_EQUAL(String(d, String::UTF16), a);
}

BOOST_AUTO_TEST_CASE(testUTF16DecodeInvalidBOM)
{
  const ByteVector b(" ", 1);
  const ByteVector c("  ", 2);
  const ByteVector d("  \0f\0o\0o", 8);
  BOOST_CHECK(String(b, String::UTF16).isEmpty());
  BOOST_CHECK(String(c, String::UTF16).isEmpty());
  BOOST_CHECK(String(d, String::UTF16).isEmpty());
}

BOOST_AUTO_TEST_CASE(testUTF16DecodeEmptyWithBOM)
{
  const ByteVector a("\377\376", 2);
  const ByteVector b("\376\377", 2);
  BOOST_CHECK(String(a, String::UTF16).isEmpty());
  BOOST_CHECK(String(b, String::UTF16).isEmpty());
}

BOOST_AUTO_TEST_CASE(testSurrogatePair)
{
  // Make sure that a surrogate pair is converted into single UTF-8 char
  // and vice versa.
  
  const ByteVector v1("\xff\xfe\x42\xd8\xb7\xdf\xce\x91\x4b\x5c");
  const ByteVector v2("\xf0\xa0\xae\xb7\xe9\x87\x8e\xe5\xb1\x8b");
  
  const String s1(v1, String::UTF16);
  BOOST_CHECK_EQUAL(s1.data(String::UTF8), v2);
  
  const String s2(v2, String::UTF8);
  BOOST_CHECK_EQUAL(s2.data(String::UTF16), v1);
  
  const ByteVector v3("\xfe\xff\xd8\x01\x30\x42");
  BOOST_CHECK(String(v3, String::UTF16).data(String::UTF8).isEmpty());
  
  const ByteVector v4("\xfe\xff\x30\x42\xdc\x01");
  BOOST_CHECK(String(v4, String::UTF16).data(String::UTF8).isEmpty());
  
  const ByteVector v5("\xfe\xff\xdc\x01\xd8\x01");
  BOOST_CHECK(String(v5, String::UTF16).data(String::UTF8).isEmpty());
}

BOOST_AUTO_TEST_CASE(testAppendCharDetach)
{
  String a("a");
  String b = a;
  a += "b";
  BOOST_CHECK_EQUAL(a, "ab");
  BOOST_CHECK_EQUAL(b, "a");
}

BOOST_AUTO_TEST_CASE(testAppendStringDetach)
{
  String a("a");
  String b = a;
  a += 'b';
  BOOST_CHECK_EQUAL(a, "ab");
  BOOST_CHECK_EQUAL(b, "a");
}

BOOST_AUTO_TEST_CASE(testToInt)
{
  bool ok;
  BOOST_CHECK_EQUAL(String("123").toInt(&ok), 123);
  BOOST_CHECK_EQUAL(ok, true);
  
  BOOST_CHECK_EQUAL(String("-123").toInt(&ok), -123);
  BOOST_CHECK_EQUAL(ok, true);
  
  BOOST_CHECK_EQUAL(String("abc").toInt(&ok), 0);
  BOOST_CHECK_EQUAL(ok, false);
  
  BOOST_CHECK_EQUAL(String("1x").toInt(&ok), 1);
  BOOST_CHECK_EQUAL(ok, false);
  
  BOOST_CHECK_EQUAL(String("").toInt(&ok), 0);
  BOOST_CHECK_EQUAL(ok, false);
  
  BOOST_CHECK_EQUAL(String("-").toInt(&ok), 0);
  BOOST_CHECK_EQUAL(ok, false);
  
  BOOST_CHECK_EQUAL(String("123").toInt(), 123);
  BOOST_CHECK_EQUAL(String("-123").toInt(), -123);
  BOOST_CHECK_EQUAL(String("123aa").toInt(), 123);
  BOOST_CHECK_EQUAL(String("-123aa").toInt(), -123);
  
  BOOST_CHECK_EQUAL(String("0000").toInt(), 0);
  BOOST_CHECK_EQUAL(String("0001").toInt(), 1);
  
  String("2147483648").toInt(&ok);
  BOOST_CHECK_EQUAL(ok, false);
  
  String("-2147483649").toInt(&ok);
  BOOST_CHECK_EQUAL(ok, false);
}

BOOST_AUTO_TEST_CASE(testFromInt)
{
  BOOST_CHECK_EQUAL(String::number(0), "0");
  BOOST_CHECK_EQUAL(String::number(12345678), "12345678");
  BOOST_CHECK_EQUAL(String::number(-12345678), "-12345678");
}

BOOST_AUTO_TEST_CASE(testSubstr)
{
  BOOST_CHECK_EQUAL(String("0123456").substr(0, 2), "01");
  BOOST_CHECK_EQUAL(String("0123456").substr(1, 2), "12");
  BOOST_CHECK_EQUAL(String("0123456").substr(1, 200), "123456");
  BOOST_CHECK_EQUAL(String("0123456").substr(0, 7), "0123456");
  BOOST_CHECK_EQUAL(String("0123456").substr(0, 200), "0123456");
}

BOOST_AUTO_TEST_CASE(testNewline)
{
  const ByteVector cr("abc\x0dxyz", 7);
  const ByteVector lf("abc\x0axyz", 7);
  const ByteVector crlf("abc\x0d\x0axyz", 8);
  
  BOOST_CHECK_EQUAL(String(cr).size(), 7);
  BOOST_CHECK_EQUAL(String(lf).size(), 7);
  BOOST_CHECK_EQUAL(String(crlf).size(), 8);
  
  BOOST_CHECK_EQUAL(String(cr)[3], L'\x0d');
  BOOST_CHECK_EQUAL(String(lf)[3], L'\x0a');
  BOOST_CHECK_EQUAL(String(crlf)[3], L'\x0d');
  BOOST_CHECK_EQUAL(String(crlf)[4], L'\x0a');
}

BOOST_AUTO_TEST_CASE(testUpper)
{
  String s1 = "tagLIB 012 strING";
  String s2 = s1.upper();
  BOOST_CHECK_EQUAL(s1, "tagLIB 012 strING");
  BOOST_CHECK_EQUAL(s2, "TAGLIB 012 STRING");
}

BOOST_AUTO_TEST_CASE(testEncodeNonLatin1)
{
  const String jpn(L"\u65E5\u672C\u8A9E");
  BOOST_CHECK_EQUAL(jpn.data(String::Latin1), "\xE5\x2C\x9E");
  BOOST_CHECK_EQUAL(jpn.data(String::UTF8), "\xE6\x97\xA5\xE6\x9C\xAC\xE8\xAA\x9E");
  BOOST_CHECK_EQUAL(jpn.data(String::UTF16), "\xFF\xFE\xE5\x65\x2C\x67\x9E\x8A");
  BOOST_CHECK_EQUAL(jpn.data(String::UTF16LE), "\xE5\x65\x2C\x67\x9E\x8A");
  BOOST_CHECK_EQUAL(jpn.data(String::UTF16BE), "\x65\xE5\x67\x2C\x8A\x9E");
  BOOST_CHECK_EQUAL(jpn.to8Bit(false), "\xE5\x2C\x9E");
  BOOST_CHECK_EQUAL(jpn.to8Bit(true), "\xE6\x97\xA5\xE6\x9C\xAC\xE8\xAA\x9E");
}

BOOST_AUTO_TEST_CASE(testEncodeEmpty)
{
    const String empty;
    BOOST_CHECK(empty.data(String::Latin1).isEmpty());
    BOOST_CHECK(empty.data(String::UTF8).isEmpty());
    BOOST_CHECK_EQUAL(empty.data(String::UTF16), "\xFF\xFE");
    BOOST_CHECK(empty.data(String::UTF16LE).isEmpty());
    BOOST_CHECK(empty.data(String::UTF16BE).isEmpty());
    BOOST_CHECK(empty.to8Bit(false).empty());
    BOOST_CHECK(empty.to8Bit(true).empty());
}

BOOST_AUTO_TEST_CASE(testEncodeNonBMP)
{
  const ByteVector a("\xFF\xFE\x3C\xD8\x50\xDD\x40\xD8\xF5\xDC\x3C\xD8\x00\xDE", 14);
  const ByteVector b("\xF0\x9F\x85\x90\xF0\xA0\x83\xB5\xF0\x9F\x88\x80");
  BOOST_CHECK_EQUAL(String(a, String::UTF16).data(String::UTF8), b);
}

BOOST_AUTO_TEST_CASE(testIterator)
{
  String s1 = "taglib string";
  String s2 = s1;
  
  String::Iterator it1 = s1.begin();
  String::Iterator it2 = s2.begin();
  
  BOOST_CHECK_EQUAL(*it1, L't');
  BOOST_CHECK_EQUAL(*it2, L't');
  
  std::advance(it1, 4);
  std::advance(it2, 4);
  *it2 = L'I';
  BOOST_CHECK_EQUAL(*it1, L'i');
  BOOST_CHECK_EQUAL(*it2, L'I');
}

BOOST_AUTO_TEST_CASE(testInvalidUTF8)
{
  BOOST_CHECK_EQUAL(String(ByteVector("\x2F"), String::UTF8), "/");
  BOOST_CHECK(String(ByteVector("\xC0\xAF"), String::UTF8).isEmpty());
  BOOST_CHECK(String(ByteVector("\xE0\x80\xAF"), String::UTF8).isEmpty());
  BOOST_CHECK(String(ByteVector("\xF0\x80\x80\xAF"), String::UTF8).isEmpty());
  
  BOOST_CHECK(String(ByteVector("\xF8\x80\x80\x80\x80"), String::UTF8).isEmpty());
  BOOST_CHECK(String(ByteVector("\xFC\x80\x80\x80\x80\x80"), String::UTF8).isEmpty());
  
  BOOST_CHECK(String(ByteVector("\xC2"), String::UTF8).isEmpty());
  BOOST_CHECK(String(ByteVector("\xE0\x80"), String::UTF8).isEmpty());
  BOOST_CHECK(String(ByteVector("\xF0\x80\x80"), String::UTF8).isEmpty());
  BOOST_CHECK(String(ByteVector("\xF8\x80\x80\x80"), String::UTF8).isEmpty());
  BOOST_CHECK(String(ByteVector("\xFC\x80\x80\x80\x80"), String::UTF8).isEmpty());
  
  BOOST_CHECK(String('\x80', String::UTF8).isEmpty());
  
  BOOST_CHECK(String(ByteVector("\xED\xA0\x80\xED\xB0\x80"), String::UTF8).isEmpty());
  BOOST_CHECK(String(ByteVector("\xED\xB0\x80\xED\xA0\x80"), String::UTF8).isEmpty());
}

BOOST_AUTO_TEST_SUITE_END()
