/***************************************************************************
    copyright           : (C) 2012 by Lukas Lalinsky
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

#include <opusfile.h>
#include <oggpageheader.h>
#include <boost/test/unit_test.hpp>
#include "utils.h"
#include "loghelpers.h"

using namespace TagLib;

BOOST_AUTO_TEST_SUITE(TestOpus)

BOOST_AUTO_TEST_CASE(testAudioProperties)
{
  Ogg::Opus::File f(TEST_FILE_PATH_C("correctness_gain_silent_output.opus"));
  BOOST_CHECK(f.audioProperties());
  BOOST_CHECK_EQUAL(f.audioProperties()->length(), 7);
  BOOST_CHECK_EQUAL(f.audioProperties()->lengthInSeconds(), 7);
  BOOST_CHECK_EQUAL(f.audioProperties()->lengthInMilliseconds(), 7737);
  BOOST_CHECK_EQUAL(f.audioProperties()->bitrate(), 37);
  BOOST_CHECK_EQUAL(f.audioProperties()->channels(), 1);
  BOOST_CHECK_EQUAL(f.audioProperties()->sampleRate(), 48000);
  BOOST_CHECK_EQUAL(f.audioProperties()->inputSampleRate(), 48000);
  BOOST_CHECK_EQUAL(f.audioProperties()->opusVersion(), 1);
}

BOOST_AUTO_TEST_CASE(testReadComments)
{
  Ogg::Opus::File f(TEST_FILE_PATH_C("correctness_gain_silent_output.opus"));
  BOOST_CHECK_EQUAL(f.tag()->fieldListMap()["ENCODER"], StringList("Xiph.Org Opus testvectormaker"));
  BOOST_CHECK(f.tag()->fieldListMap().contains("TESTDESCRIPTION"));
  BOOST_CHECK(!f.tag()->fieldListMap().contains("ARTIST"));
  BOOST_CHECK_EQUAL(f.tag()->vendorID(), "libopus 0.9.11-66-g64c2dd7");
}

BOOST_AUTO_TEST_CASE(testWriteComments)
{
  const ScopedFileCopy copy("correctness_gain_silent_output", ".opus");
  {
    Ogg::Opus::File f(copy.fileName());
    f.tag()->setArtist("Your Tester");
    f.save();
  }
  {
    Ogg::Opus::File f(copy.fileName());
    BOOST_CHECK_EQUAL(f.tag()->fieldListMap()["ENCODER"], StringList("Xiph.Org Opus testvectormaker"));
    BOOST_CHECK(f.tag()->fieldListMap().contains("TESTDESCRIPTION"));
    BOOST_CHECK_EQUAL(f.tag()->fieldListMap()["ARTIST"], StringList("Your Tester"));
    BOOST_CHECK_EQUAL(f.tag()->vendorID(), "libopus 0.9.11-66-g64c2dd7");
  }
}

BOOST_AUTO_TEST_CASE(testSplitPackets)
{
  const ScopedFileCopy copy("correctness_gain_silent_output", ".opus");
  const String text = longText(128 * 1024, true);
  {
    Ogg::Opus::File f(copy.fileName());
    f.tag()->setTitle(text);
    f.save();
  }
  {
    Ogg::Opus::File f(copy.fileName());
    BOOST_CHECK(f.isValid());
    BOOST_CHECK_EQUAL(f.length(), 167534);
    BOOST_CHECK_EQUAL(f.lastPageHeader()->pageSequenceNumber(), 27);
    BOOST_CHECK_EQUAL(f.packet(0).size(), 19);
    BOOST_CHECK_EQUAL(f.packet(1).size(), 131380);
    BOOST_CHECK_EQUAL(f.packet(2).size(), 5);
    BOOST_CHECK_EQUAL(f.packet(3).size(), 5);
    BOOST_CHECK_EQUAL(f.tag()->title(), text);
  
    BOOST_CHECK(f.audioProperties());
    BOOST_CHECK_EQUAL(f.audioProperties()->lengthInMilliseconds(), 7737);
  
    f.tag()->setTitle("ABCDE");
    f.save();
  }
  {
    Ogg::Opus::File f(copy.fileName());
    BOOST_CHECK(f.isValid());
    BOOST_CHECK_EQUAL(f.length(), 35521);
    BOOST_CHECK_EQUAL(f.lastPageHeader()->pageSequenceNumber(), 11);
    BOOST_CHECK_EQUAL(f.packet(0).size(), 19);
    BOOST_CHECK_EQUAL(f.packet(1).size(), 313);
    BOOST_CHECK_EQUAL(f.packet(2).size(), 5);
    BOOST_CHECK_EQUAL(f.packet(3).size(), 5);
    BOOST_CHECK_EQUAL(f.tag()->title(), "ABCDE");
  
    BOOST_CHECK(f.audioProperties());
    BOOST_CHECK_EQUAL(f.audioProperties()->lengthInMilliseconds(), 7737);
  }
}

BOOST_AUTO_TEST_SUITE_END()
