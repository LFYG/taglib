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

#define _USE_MATH_DEFINES
#include <cmath>
#include <tbytevector.h>
#include <tbytevectorlist.h>
#include <boost/test/unit_test.hpp>
#include "loghelpers.h"

using namespace TagLib;

BOOST_AUTO_TEST_SUITE(TestByteVector)

BOOST_AUTO_TEST_CASE(testByteVector)
{
  const ByteVector s1("foo");
  BOOST_CHECK_EQUAL(ByteVectorList::split(s1, " ").size(), 1);
  
  const ByteVector s2("f");
  BOOST_CHECK_EQUAL(ByteVectorList::split(s2, " ").size(), 1);

  BOOST_CHECK(ByteVector().isEmpty());
  BOOST_CHECK_EQUAL(0, ByteVector().size());
  BOOST_CHECK(ByteVector("asdf").clear().isEmpty());
  BOOST_CHECK_EQUAL(0, ByteVector("asdf").clear().size());
  BOOST_CHECK_EQUAL(ByteVector(), ByteVector("asdf").clear());
  
  ByteVector i("blah blah");
  const ByteVector j("blah");
  BOOST_CHECK(i.containsAt(j, 5, 0));
  BOOST_CHECK(i.containsAt(j, 6, 1));
  BOOST_CHECK(i.containsAt(j, 6, 1, 3));
  
  i.clear();
  BOOST_CHECK(i.isEmpty());
  BOOST_CHECK(!i.isNull()); // deprecated, but worth it to check.
}

BOOST_AUTO_TEST_CASE(testFind1)
{
  BOOST_CHECK_EQUAL(ByteVector("....SggO.").find("SggO"), 4);
  BOOST_CHECK_EQUAL(ByteVector("....SggO.").find("SggO", 0), 4);
  BOOST_CHECK_EQUAL(ByteVector("....SggO.").find("SggO", 1), 4);
  BOOST_CHECK_EQUAL(ByteVector("....SggO.").find("SggO", 2), 4);
  BOOST_CHECK_EQUAL(ByteVector("....SggO.").find("SggO", 3), 4);
  BOOST_CHECK_EQUAL(ByteVector("....SggO.").find("SggO", 4), 4);
  BOOST_CHECK_EQUAL(ByteVector("....SggO.").find("SggO", 5), -1);
  BOOST_CHECK_EQUAL(ByteVector("....SggO.").find("SggO", 6), -1);
  BOOST_CHECK_EQUAL(ByteVector("....SggO.").find("SggO", 7), -1);
  BOOST_CHECK_EQUAL(ByteVector("....SggO.").find("SggO", 8), -1);

  // Intentional out-of-bounds access.
  ByteVector v("0123456789x");
  v.resize(10);
  v.data()[10] = 'x';
  BOOST_CHECK_EQUAL(v.find("789x", 7), -1);
}

BOOST_AUTO_TEST_CASE(testFind2)
{
  BOOST_CHECK_EQUAL(ByteVector("\x01", 1).find("\x01"), 0);
  BOOST_CHECK_EQUAL(ByteVector("\x01\x02", 2).find("\x01\x02"), 0);
  BOOST_CHECK_EQUAL(ByteVector("\x01", 1).find("\x02"), -1);
  BOOST_CHECK_EQUAL(ByteVector("\x01\x02", 2).find("\x01\x03"), -1);
}

BOOST_AUTO_TEST_CASE(testFind3)
{
  BOOST_CHECK_EQUAL(ByteVector("....SggO.").find('S'), 4);
  BOOST_CHECK_EQUAL(ByteVector("....SggO.").find('S', 0), 4);
  BOOST_CHECK_EQUAL(ByteVector("....SggO.").find('S', 1), 4);
  BOOST_CHECK_EQUAL(ByteVector("....SggO.").find('S', 2), 4);
  BOOST_CHECK_EQUAL(ByteVector("....SggO.").find('S', 3), 4);
  BOOST_CHECK_EQUAL(ByteVector("....SggO.").find('S', 4), 4);
  BOOST_CHECK_EQUAL(ByteVector("....SggO.").find('S', 5), -1);
  BOOST_CHECK_EQUAL(ByteVector("....SggO.").find('S', 6), -1);
  BOOST_CHECK_EQUAL(ByteVector("....SggO.").find('S', 7), -1);
  BOOST_CHECK_EQUAL(ByteVector("....SggO.").find('S', 8), -1);
}

BOOST_AUTO_TEST_CASE(testRfind1)
{
  BOOST_CHECK_EQUAL(ByteVector(".OggS....").rfind("OggS", 0), 1);
  BOOST_CHECK_EQUAL(ByteVector(".OggS....").rfind("OggS", 1), 1);
  BOOST_CHECK_EQUAL(ByteVector(".OggS....").rfind("OggS", 2), 1);
  BOOST_CHECK_EQUAL(ByteVector(".OggS....").rfind("OggS", 3), 1);
  BOOST_CHECK_EQUAL(ByteVector(".OggS....").rfind("OggS", 4), 1);
  BOOST_CHECK_EQUAL(ByteVector(".OggS....").rfind("OggS", 5), 1);
  BOOST_CHECK_EQUAL(ByteVector(".OggS....").rfind("OggS", 6), 1);
  BOOST_CHECK_EQUAL(ByteVector(".OggS....").rfind("OggS", 7), 1);
  BOOST_CHECK_EQUAL(ByteVector(".OggS....").rfind("OggS", 8), 1);
  BOOST_CHECK_EQUAL(ByteVector(".OggS....").rfind("OggS"), 1);
}

BOOST_AUTO_TEST_CASE(testRfind2)
{
  const ByteVector r0("**************");
  const ByteVector r1("OggS**********");
  const ByteVector r2("**********OggS");
  const ByteVector r3("OggS******OggS");
  const ByteVector r4("OggS*OggS*OggS");
  
  BOOST_CHECK_EQUAL(r0.find("OggS"), -1);
  BOOST_CHECK_EQUAL(r0.rfind("OggS"), -1);
  BOOST_CHECK_EQUAL(r1.find("OggS"), 0);
  BOOST_CHECK_EQUAL(r1.rfind("OggS"), 0);
  BOOST_CHECK_EQUAL(r2.find("OggS"), 10);
  BOOST_CHECK_EQUAL(r2.rfind("OggS"), 10);
  BOOST_CHECK_EQUAL(r3.find("OggS"), 0);
  BOOST_CHECK_EQUAL(r3.rfind("OggS"), 10);
  BOOST_CHECK_EQUAL(r4.rfind("OggS"), 10);
  BOOST_CHECK_EQUAL(r4.rfind("OggS", 0), 10);
  BOOST_CHECK_EQUAL(r4.rfind("OggS", 7), 5);
  BOOST_CHECK_EQUAL(r4.rfind("OggS", 12), 10);
}

BOOST_AUTO_TEST_CASE(testRfind3)
{
  BOOST_CHECK_EQUAL(ByteVector(".OggS....").rfind('O', 0), 1);
  BOOST_CHECK_EQUAL(ByteVector(".OggS....").rfind('O', 1), 1);
  BOOST_CHECK_EQUAL(ByteVector(".OggS....").rfind('O', 2), 1);
  BOOST_CHECK_EQUAL(ByteVector(".OggS....").rfind('O', 3), 1);
  BOOST_CHECK_EQUAL(ByteVector(".OggS....").rfind('O', 4), 1);
  BOOST_CHECK_EQUAL(ByteVector(".OggS....").rfind('O', 5), 1);
  BOOST_CHECK_EQUAL(ByteVector(".OggS....").rfind('O', 6), 1);
  BOOST_CHECK_EQUAL(ByteVector(".OggS....").rfind('O', 7), 1);
  BOOST_CHECK_EQUAL(ByteVector(".OggS....").rfind('O', 8), 1);
  BOOST_CHECK_EQUAL(ByteVector(".OggS....").rfind('O'), 1);
}

BOOST_AUTO_TEST_CASE(testToHex)
{
  const ByteVector v("\xf0\xe1\xd2\xc3\xb4\xa5\x96\x87\x78\x69\x5a\x4b\x3c\x2d\x1e\x0f", 16);
  BOOST_CHECK_EQUAL(v.toHex(), "f0e1d2c3b4a5968778695a4b3c2d1e0f");
}

BOOST_AUTO_TEST_CASE(testIntegerConversion)
{
  const ByteVector data("\x00\xff\x01\xff\x00\xff\x01\xff\x00\xff\x01\xff\x00\xff", 14);
  
  BOOST_CHECK_EQUAL(data.toShort(), (short)0x00ff);
  BOOST_CHECK_EQUAL(data.toShort(false), (short)0xff00);
  BOOST_CHECK_EQUAL(data.toShort(5U), (short)0xff01);
  BOOST_CHECK_EQUAL(data.toShort(5U, false), (short)0x01ff);
  BOOST_CHECK_EQUAL(data.toShort(13U), (short)0xff);
  BOOST_CHECK_EQUAL(data.toShort(13U, false), (short)0xff);
  
  BOOST_CHECK_EQUAL(data.toUShort(), (unsigned short)0x00ff);
  BOOST_CHECK_EQUAL(data.toUShort(false), (unsigned short)0xff00);
  BOOST_CHECK_EQUAL(data.toUShort(5U), (unsigned short)0xff01);
  BOOST_CHECK_EQUAL(data.toUShort(5U, false), (unsigned short)0x01ff);
  BOOST_CHECK_EQUAL(data.toUShort(13U), (unsigned short)0xff);
  BOOST_CHECK_EQUAL(data.toUShort(13U, false), (unsigned short)0xff);
  
  BOOST_CHECK_EQUAL(data.toUInt(), 0x00ff01ffU);
  BOOST_CHECK_EQUAL(data.toUInt(false), 0xff01ff00U);
  BOOST_CHECK_EQUAL(data.toUInt(5U), 0xff01ff00U);
  BOOST_CHECK_EQUAL(data.toUInt(5U, false), 0x00ff01ffU);
  BOOST_CHECK_EQUAL(data.toUInt(12U), 0x00ffU);
  BOOST_CHECK_EQUAL(data.toUInt(12U, false), 0xff00U);
  
  BOOST_CHECK_EQUAL(data.toUInt(0U, 3U), 0x00ff01U);
  BOOST_CHECK_EQUAL(data.toUInt(0U, 3U, false), 0x01ff00U);
  BOOST_CHECK_EQUAL(data.toUInt(5U, 3U), 0xff01ffU);
  BOOST_CHECK_EQUAL(data.toUInt(5U, 3U, false), 0xff01ffU);
  BOOST_CHECK_EQUAL(data.toUInt(12U, 3U), 0x00ffU);
  BOOST_CHECK_EQUAL(data.toUInt(12U, 3U, false), 0xff00U);
  
  BOOST_CHECK_EQUAL(data.toLongLong(), (long long)0x00ff01ff00ff01ffULL);
  BOOST_CHECK_EQUAL(data.toLongLong(false), (long long)0xff01ff00ff01ff00ULL);
  BOOST_CHECK_EQUAL(data.toLongLong(5U), (long long)0xff01ff00ff01ff00ULL);
  BOOST_CHECK_EQUAL(data.toLongLong(5U, false), (long long)0x00ff01ff00ff01ffULL);
  BOOST_CHECK_EQUAL(data.toLongLong(12U), (long long)0x00ffU);
  BOOST_CHECK_EQUAL(data.toLongLong(12U, false), (long long)0xff00U);
}

BOOST_AUTO_TEST_CASE(testFloatingPointConversion)
{
  const double Tolerance = 1.0e-7;
  
  const ByteVector pi32le("\xdb\x0f\x49\x40", 4);
  BOOST_CHECK(std::abs(pi32le.toFloat32LE(0) - M_PI) < Tolerance);
  BOOST_CHECK_EQUAL(ByteVector::fromFloat32LE(pi32le.toFloat32LE(0)), pi32le);
  
  const ByteVector pi32be("\x40\x49\x0f\xdb", 4);
  BOOST_CHECK(std::abs(pi32be.toFloat32BE(0) - M_PI) < Tolerance);
  BOOST_CHECK_EQUAL(ByteVector::fromFloat32BE(pi32be.toFloat32BE(0)), pi32be);
  
  const ByteVector pi64le("\x18\x2d\x44\x54\xfb\x21\x09\x40", 8);
  BOOST_CHECK(std::abs(pi64le.toFloat64LE(0) - M_PI) < Tolerance);
  BOOST_CHECK_EQUAL(ByteVector::fromFloat64LE(pi64le.toFloat64LE(0)), pi64le);
  
  const ByteVector pi64be("\x40\x09\x21\xfb\x54\x44\x2d\x18", 8);
  BOOST_CHECK(std::abs(pi64be.toFloat64BE(0) - M_PI) < Tolerance);
  BOOST_CHECK_EQUAL(ByteVector::fromFloat64BE(pi64be.toFloat64BE(0)), pi64be);
  
  const ByteVector pi80le("\x00\xc0\x68\x21\xa2\xda\x0f\xc9\x00\x40", 10);
  BOOST_CHECK(std::abs(pi80le.toFloat80LE(0) - M_PI) < Tolerance);
  
  const ByteVector pi80be("\x40\x00\xc9\x0f\xda\xa2\x21\x68\xc0\x00", 10);
  BOOST_CHECK(std::abs(pi80be.toFloat80BE(0) - M_PI) < Tolerance);
}

BOOST_AUTO_TEST_CASE(testReplace)
{
  {
    ByteVector a("abcdabf");
    a.replace("", "<a>");
    BOOST_CHECK_EQUAL(a, "abcdabf");
  }
  {
    ByteVector a("abcdabf");
    a.replace("foobartoolong", "<a>");
    BOOST_CHECK_EQUAL(a, "abcdabf");
  }
  {
    ByteVector a("abcdabf");
    a.replace("xx", "yy");
    BOOST_CHECK_EQUAL(a, "abcdabf");
  }
  {
    ByteVector a("abcdabf");
    a.replace("a", "x");
    BOOST_CHECK_EQUAL(a, "xbcdxbf");
    a.replace("x", "a");
    BOOST_CHECK_EQUAL(a, "abcdabf");
  }
  {
    ByteVector a("abcdabf");
    a.replace('a', 'x');
    BOOST_CHECK_EQUAL(a, "xbcdxbf");
    a.replace('x', 'a');
    BOOST_CHECK_EQUAL(a, "abcdabf");
  }
  {
    ByteVector a("abcdabf");
    a.replace("ab", "xy");
    BOOST_CHECK_EQUAL(a, "xycdxyf");
    a.replace("xy", "ab");
    BOOST_CHECK_EQUAL(a, "abcdabf");
  }
  {
    ByteVector a("abcdabf");
    a.replace("a", "<a>");
    BOOST_CHECK_EQUAL(a, "<a>bcd<a>bf");
    a.replace("<a>", "a");
    BOOST_CHECK_EQUAL(a, "abcdabf");
  }
  {
    ByteVector a("abcdabf");
    a.replace("b", "<b>");
    BOOST_CHECK_EQUAL(a, "a<b>cda<b>f");
    a.replace("<b>", "b");
    BOOST_CHECK_EQUAL(a, "abcdabf");
  }
  {
    ByteVector a("abcdabc");
    a.replace("c", "<c>");
    BOOST_CHECK_EQUAL(a, "ab<c>dab<c>");
    a.replace("<c>", "c");
    BOOST_CHECK_EQUAL(a, "abcdabc");
  }
  {
    ByteVector a("abcdaba");
    a.replace("a", "<a>");
    BOOST_CHECK_EQUAL(a, "<a>bcd<a>b<a>");
    a.replace("<a>", "a");
    BOOST_CHECK_EQUAL(a, "abcdaba");
  }
}

BOOST_AUTO_TEST_CASE(testReplaceAndDetach)
{
  {
    ByteVector a("abcdabf");
    ByteVector b = a;
    a.replace("a", "x");
    BOOST_CHECK_EQUAL(a, "xbcdxbf");
    BOOST_CHECK_EQUAL(b, "abcdabf");
  }
  {
    ByteVector a("abcdabf");
    ByteVector b = a;
    a.replace('a', 'x');
    BOOST_CHECK_EQUAL(a, "xbcdxbf");
    BOOST_CHECK_EQUAL(b, "abcdabf");
  }
  {
    ByteVector a("abcdabf");
    ByteVector b = a;
    a.replace("ab", "xy");
    BOOST_CHECK_EQUAL(a, "xycdxyf");
    BOOST_CHECK_EQUAL(b, "abcdabf");
  }
  {
    ByteVector a("abcdabf");
    ByteVector b = a;
    a.replace("a", "<a>");
    BOOST_CHECK_EQUAL(a, "<a>bcd<a>bf");
    BOOST_CHECK_EQUAL(b, "abcdabf");
  }
  {
    ByteVector a("ab<c>dab<c>");
    ByteVector b = a;
    a.replace("<c>", "c");
    BOOST_CHECK_EQUAL(a, "abcdabc");
    BOOST_CHECK_EQUAL(b, "ab<c>dab<c>");
  }
}

BOOST_AUTO_TEST_CASE(testIterator)
{
  ByteVector v1("taglib");
  ByteVector v2 = v1;
  
  ByteVector::Iterator it1 = v1.begin();
  ByteVector::Iterator it2 = v2.begin();
  
  BOOST_CHECK_EQUAL(*it1, 't');
  BOOST_CHECK_EQUAL(*it2, 't');
  
  std::advance(it1, 4);
  std::advance(it2, 4);
  *it2 = 'I';
  BOOST_CHECK_EQUAL(*it1, 'i');
  BOOST_CHECK_EQUAL(*it2, 'I');
  BOOST_CHECK_EQUAL(v1, "taglib");
  BOOST_CHECK_EQUAL(v2, "taglIb");
  
  ByteVector::ReverseIterator it3 = v1.rbegin();
  ByteVector::ReverseIterator it4 = v2.rbegin();
  
  BOOST_CHECK_EQUAL(*it3, 'b');
  BOOST_CHECK_EQUAL(*it4, 'b');
  
  std::advance(it3, 4);
  std::advance(it4, 4);
  *it4 = 'A';
  BOOST_CHECK_EQUAL(*it3, 'a');
  BOOST_CHECK_EQUAL(*it4, 'A');
  BOOST_CHECK_EQUAL(v1, "taglib");
  BOOST_CHECK_EQUAL(v2, "tAglIb");
  
  ByteVector v3;
  v3 = ByteVector("0123456789").mid(3, 4);
  
  it1 = v3.begin();
  it2 = v3.end() - 1;
  BOOST_CHECK_EQUAL(*it1, '3');
  BOOST_CHECK_EQUAL(*it2, '6');
  
  it3 = v3.rbegin();
  it4 = v3.rend() - 1;
  BOOST_CHECK_EQUAL(*it3, '6');
  BOOST_CHECK_EQUAL(*it4, '3');
}

BOOST_AUTO_TEST_CASE(testResize)
{
  ByteVector a = ByteVector("0123456789");
  ByteVector b = a.mid(3, 4);
  b.resize(6, 'A');
  BOOST_CHECK_EQUAL(b.size(), 6);
  BOOST_CHECK_EQUAL(b[3], '6');
  BOOST_CHECK_EQUAL(b[4], 'A');
  BOOST_CHECK_EQUAL(b[5], 'A');
  b.resize(10, 'B');
  BOOST_CHECK_EQUAL(b.size(), 10);
  BOOST_CHECK_EQUAL(b[3], '6');
  BOOST_CHECK_EQUAL(b[6], 'B');
  BOOST_CHECK_EQUAL(b[9], 'B');
  b.resize(3, 'C');
  BOOST_CHECK_EQUAL(b.size(), 3);
  BOOST_CHECK_EQUAL(b.find('C'), -1);
  b.resize(3);
  BOOST_CHECK_EQUAL(b.size(), 3);
  
  // Check if a and b were properly detached.
  
  BOOST_CHECK_EQUAL(a.size(), 10);
  BOOST_CHECK_EQUAL(a[3], '3');
  BOOST_CHECK_EQUAL(a[5], '5');
  
  // Special case that refCount == 1 and d->offset != 0.
  
  ByteVector c = ByteVector("0123456789").mid(3, 4);
  c.resize(6, 'A');
  BOOST_CHECK_EQUAL(c.size(), 6);
  BOOST_CHECK_EQUAL(c[3], '6');
  BOOST_CHECK_EQUAL(c[4], 'A');
  BOOST_CHECK_EQUAL(c[5], 'A');
  c.resize(10, 'B');
  BOOST_CHECK_EQUAL(c.size(), 10);
  BOOST_CHECK_EQUAL(c[3], '6');
  BOOST_CHECK_EQUAL(c[6], 'B');
  BOOST_CHECK_EQUAL(c[9], 'B');
  c.resize(3, 'C');
  BOOST_CHECK_EQUAL(c.size(), 3);
  BOOST_CHECK_EQUAL(c.find('C'), -1);
}

BOOST_AUTO_TEST_CASE(testAppend1)
{
  ByteVector v1("foo");
  v1.append("bar");
  BOOST_CHECK_EQUAL(v1, "foobar");
  
  ByteVector v2("foo");
  v2.append("b");
  BOOST_CHECK_EQUAL(v2, "foob");
  
  ByteVector v3;
  v3.append("b");
  BOOST_CHECK_EQUAL(v3, "b");
  
  ByteVector v4("foo");
  v4.append(v1);
  BOOST_CHECK_EQUAL(v4, "foofoobar");
  
  ByteVector v5("foo");
  v5.append('b');
  BOOST_CHECK_EQUAL(v5, "foob");
  
  ByteVector v6;
  v6.append('b');
  BOOST_CHECK_EQUAL(v6, "b");
  
  ByteVector v7("taglib");
  ByteVector v8 = v7;
  
  v7.append("ABC");
  BOOST_CHECK_EQUAL(v7, "taglibABC");
  v7.append('1');
  v7.append('2');
  v7.append('3');
  BOOST_CHECK_EQUAL(v7, "taglibABC123");
  BOOST_CHECK_EQUAL(v8, "taglib");
}

BOOST_AUTO_TEST_CASE(testAppend2)
{
  ByteVector a("1234");
  a.append(a);
  BOOST_CHECK_EQUAL(a, "12341234");
}

BOOST_AUTO_TEST_CASE(testBase64)
{
  ByteVector sempty;
  ByteVector t0("a"); // test 1 byte
  ByteVector t1("any carnal pleasure.");
  ByteVector t2("any carnal pleasure");
  ByteVector t3("any carnal pleasur");
  ByteVector s0("a"); // test 1 byte
  ByteVector s1("any carnal pleasure.");
  ByteVector s2("any carnal pleasure");
  ByteVector s3("any carnal pleasur");
  ByteVector eempty;
  ByteVector e0("YQ==");
  ByteVector e1("YW55IGNhcm5hbCBwbGVhc3VyZS4=");
  ByteVector e2("YW55IGNhcm5hbCBwbGVhc3VyZQ==");
  ByteVector e3("YW55IGNhcm5hbCBwbGVhc3Vy");
  
  // Encode
  BOOST_CHECK_EQUAL(sempty.toBase64(), eempty);
  BOOST_CHECK_EQUAL(s0.toBase64(), e0);
  BOOST_CHECK_EQUAL(s1.toBase64(), e1);
  BOOST_CHECK_EQUAL(s2.toBase64(), e2);
  BOOST_CHECK_EQUAL(s3.toBase64(), e3);
  
  // Decode
  BOOST_CHECK_EQUAL(ByteVector::fromBase64(eempty), sempty);
  BOOST_CHECK_EQUAL(ByteVector::fromBase64(e0), s0);
  BOOST_CHECK_EQUAL(ByteVector::fromBase64(e1), s1);
  BOOST_CHECK_EQUAL(ByteVector::fromBase64(e2), s2);
  BOOST_CHECK_EQUAL(ByteVector::fromBase64(e3), s3);
  
  BOOST_CHECK_EQUAL(ByteVector::fromBase64(s0.toBase64()), t0);
  BOOST_CHECK_EQUAL(ByteVector::fromBase64(s1.toBase64()), t1);
  BOOST_CHECK_EQUAL(ByteVector::fromBase64(s2.toBase64()), t2);
  BOOST_CHECK_EQUAL(ByteVector::fromBase64(s3.toBase64()), t3);
  
  ByteVector all((unsigned int)256);
  
  // in order
  {
    for(int i = 0; i < 256; i++){
      all[i]=(unsigned char)i;
      }
    ByteVector b64 = all.toBase64();
    ByteVector original = ByteVector::fromBase64(b64);
    BOOST_CHECK_EQUAL(original, all);
  }
  
  // reverse
  {
    for(int i = 0; i < 256; i++){
      all[i]=(unsigned char)255-i;
      }
    ByteVector b64 = all.toBase64();
    ByteVector original = ByteVector::fromBase64(b64);
    BOOST_CHECK_EQUAL(original, all);
  }
  
  // all zeroes
  {
    for(int i = 0; i < 256; i++){
      all[i]=0;
      }
    ByteVector b64 = all.toBase64();
    ByteVector original = ByteVector::fromBase64(b64);
    BOOST_CHECK_EQUAL(original, all);
  }
  
  // all ones
  {
    for(int i = 0; i < 256; i++){
      all[i]=(unsigned char)0xff;
      }
    ByteVector b64 = all.toBase64();
    ByteVector original = ByteVector::fromBase64(b64);
    BOOST_CHECK_EQUAL(original, all);
  }
  
  // Missing end bytes
  {
    // No missing bytes
    ByteVector m0("YW55IGNhcm5hbCBwbGVhc3VyZQ==");
    BOOST_CHECK_EQUAL(ByteVector::fromBase64(m0), s2);
  
    // 1 missing byte
    ByteVector m1("YW55IGNhcm5hbCBwbGVhc3VyZQ=");
    BOOST_CHECK_EQUAL(ByteVector::fromBase64(m1), sempty);
  
    // 2 missing bytes
    ByteVector m2("YW55IGNhcm5hbCBwbGVhc3VyZQ");
    BOOST_CHECK_EQUAL(ByteVector::fromBase64(m2), sempty);
  
    // 3 missing bytes
    ByteVector m3("YW55IGNhcm5hbCBwbGVhc3VyZ");
    BOOST_CHECK_EQUAL(ByteVector::fromBase64(m3), sempty);
  }
  
  // Grok invalid characters
  {
    ByteVector invalid("abd\x00\x01\x02\x03\x04");
    BOOST_CHECK_EQUAL(ByteVector::fromBase64(invalid), sempty);
  }
}

BOOST_AUTO_TEST_SUITE_END()
