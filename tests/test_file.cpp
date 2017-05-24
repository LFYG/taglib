/***************************************************************************
    copyright           : (C) 2015 by Tsuda Kageyu
    email               : tsuda.kageyu@gmail.com
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

#include <tfile.h>
#include <boost/test/unit_test.hpp>
#include "utils.h"

using namespace TagLib;

namespace 
{
  // File subclass that gives tests access to filesystem operations
  class PlainFile : public File {
  public:
    PlainFile(FileName name) : File(name) { }
    Tag *tag() const { return NULL; }
    AudioProperties *audioProperties() const { return NULL; }
    bool save() { return false; }
    void truncate(long length) { File::truncate(length); }
  };
}

BOOST_AUTO_TEST_SUITE(TestFile)

BOOST_AUTO_TEST_CASE(testFindInSmallFile)
{
  const ScopedFileCopy copy("empty", ".ogg");
  {
    PlainFile file(copy.fileName());
    file.seek(0);
    file.writeBlock(ByteVector("0123456239", 10));
    file.truncate(10);
  }
  {
    PlainFile file(copy.fileName());
    BOOST_CHECK_EQUAL(file.length(), 10);
  
    BOOST_CHECK_EQUAL(file.find(ByteVector("23", 2)), 2);
    BOOST_CHECK_EQUAL(file.find(ByteVector("23", 2), 2), 2);
    BOOST_CHECK_EQUAL(file.find(ByteVector("23", 2), 3), 7);
  
    file.seek(0);
    const ByteVector v = file.readBlock(file.length());
    BOOST_CHECK_EQUAL(v.size(), 10);
  
    BOOST_CHECK_EQUAL(v.find("23"),    file.find("23"));
    BOOST_CHECK_EQUAL(v.find("23", 2), file.find("23", 2));
    BOOST_CHECK_EQUAL(v.find("23", 3), file.find("23", 3));
  }
}

BOOST_AUTO_TEST_CASE(testRFindInSmallFile)
{
  const ScopedFileCopy copy("empty", ".ogg");
  {
    PlainFile file(copy.fileName());
    file.seek(0);
    file.writeBlock(ByteVector("0123456239", 10));
    file.truncate(10);
  }
  {
    PlainFile file(copy.fileName());
    BOOST_CHECK_EQUAL(file.length(), 10);
  
    BOOST_CHECK_EQUAL(file.rfind(ByteVector("23", 2)), 7);
    BOOST_CHECK_EQUAL(file.rfind(ByteVector("23", 2), 7), 7);
    BOOST_CHECK_EQUAL(file.rfind(ByteVector("23", 2), 6), 2);
  
    file.seek(0);
    const ByteVector v = file.readBlock(file.length());
    BOOST_CHECK_EQUAL(v.size(), (unsigned int)10);
  
    BOOST_CHECK_EQUAL(v.rfind("23"),    file.rfind("23"));
    BOOST_CHECK_EQUAL(v.rfind("23", 7), file.rfind("23", 7));
    BOOST_CHECK_EQUAL(v.rfind("23", 6), file.rfind("23", 6));
  }
}

BOOST_AUTO_TEST_CASE(testSeek)
{
  const ScopedFileCopy copy("empty", ".ogg");

  PlainFile f(copy.fileName());
  BOOST_CHECK_EQUAL(f.tell(), 0);
  BOOST_CHECK_EQUAL(f.length(), 4328);
  
  f.seek(100, File::Beginning);
  BOOST_CHECK_EQUAL(f.tell(), 100);
  f.seek(100, File::Current);
  BOOST_CHECK_EQUAL(f.tell(), 200);
  f.seek(-300, File::Current);
  BOOST_CHECK_EQUAL(f.tell(), 200);
  
  f.seek(-100, File::End);
  BOOST_CHECK_EQUAL(f.tell(), 4228);
  f.seek(-100, File::Current);
  BOOST_CHECK_EQUAL(f.tell(), 4128);
  f.seek(300, File::Current);
  BOOST_CHECK_EQUAL(f.tell(), 4428);
}

BOOST_AUTO_TEST_CASE(testTruncate)
{
  const ScopedFileCopy copy("empty", ".ogg");
  {
    PlainFile f(copy.fileName());
    BOOST_CHECK_EQUAL(f.length(), 4328);

    f.truncate(2000);
    BOOST_CHECK_EQUAL(f.length(), 2000);
  }
  {
    PlainFile f(copy.fileName());
    BOOST_CHECK_EQUAL(f.length(), 2000);
  }
}

BOOST_AUTO_TEST_SUITE_END()
