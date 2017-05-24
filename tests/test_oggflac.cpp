/***************************************************************************
    copyright           : (C) 2009 by Lukas Lalinsky
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

#include <oggflacfile.h>
#include <oggpageheader.h>
#include <boost/test/unit_test.hpp>
#include "utils.h"
#include "loghelpers.h"

using namespace TagLib;

BOOST_AUTO_TEST_SUITE(TestOggFLAC)

BOOST_AUTO_TEST_CASE(testFramingBit)
{
  const ScopedFileCopy copy("empty_flac", ".oga");
  {
    Ogg::FLAC::File f(copy.fileName());
    f.tag()->setArtist("The Artist");
    f.save();
  }
  {
    Ogg::FLAC::File f(copy.fileName());
    BOOST_CHECK_EQUAL(f.tag()->artist(), "The Artist");
  
    f.seek(0, File::End);
    BOOST_CHECK_EQUAL(f.tell(), 9134);
  }
}

BOOST_AUTO_TEST_CASE(testFuzzedFile)
{
  Ogg::FLAC::File f(TEST_FILE_PATH_C("segfault.oga"));
  BOOST_CHECK(!f.isValid());
}

BOOST_AUTO_TEST_CASE(testSplitPackets)
{
  const ScopedFileCopy copy("empty_flac", ".oga");
  const String text = longText(128 * 1024, true);
  {
    Ogg::FLAC::File f(copy.fileName());
    f.tag()->setTitle(text);
    f.save();
  }
  {
    Ogg::FLAC::File f(copy.fileName());
    BOOST_CHECK(f.isValid());
    BOOST_CHECK_EQUAL(f.length(), 141141);
    BOOST_CHECK_EQUAL(f.lastPageHeader()->pageSequenceNumber(), 21);
    BOOST_CHECK_EQUAL(f.packet(0).size(), 51);
    BOOST_CHECK_EQUAL(f.packet(1).size(), 131126);
    BOOST_CHECK_EQUAL(f.packet(2).size(), 22);
    BOOST_CHECK_EQUAL(f.packet(3).size(), 8196);
    BOOST_CHECK_EQUAL(f.tag()->title(), text);
  
    BOOST_CHECK(f.audioProperties());
    BOOST_CHECK_EQUAL(f.audioProperties()->lengthInMilliseconds(), 3705);
  
    f.tag()->setTitle("ABCDE");
    f.save();
  }
  {
    Ogg::FLAC::File f(copy.fileName());
    BOOST_CHECK(f.isValid());
    BOOST_CHECK_EQUAL(f.length(), 9128);
    BOOST_CHECK_EQUAL(f.lastPageHeader()->pageSequenceNumber(), 5);
    BOOST_CHECK_EQUAL(f.packet(0).size(), 51);
    BOOST_CHECK_EQUAL(f.packet(1).size(), 59);
    BOOST_CHECK_EQUAL(f.packet(2).size(), 22);
    BOOST_CHECK_EQUAL(f.packet(3).size(), 8196);
    BOOST_CHECK_EQUAL(f.tag()->title(), "ABCDE");
  
    BOOST_CHECK(f.audioProperties());
    BOOST_CHECK_EQUAL(f.audioProperties()->lengthInMilliseconds(), 3705);
  }
}

BOOST_AUTO_TEST_SUITE_END()
