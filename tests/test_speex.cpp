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

#include <speexfile.h>
#include <oggpageheader.h>
#include <boost/test/unit_test.hpp>
#include "utils.h"
#include "loghelpers.h"

using namespace TagLib;

BOOST_AUTO_TEST_SUITE(TestSpeex)

BOOST_AUTO_TEST_CASE(testAudioProperties)
{
  Ogg::Speex::File f(TEST_FILE_PATH_C("empty.spx"));
  BOOST_CHECK(f.audioProperties());
  BOOST_CHECK_EQUAL(f.audioProperties()->length(), 3);
  BOOST_CHECK_EQUAL(f.audioProperties()->lengthInSeconds(), 3);
  BOOST_CHECK_EQUAL(f.audioProperties()->lengthInMilliseconds(), 3685);
  BOOST_CHECK_EQUAL(f.audioProperties()->bitrate(), 53);
  BOOST_CHECK_EQUAL(f.audioProperties()->bitrateNominal(), -1);
  BOOST_CHECK_EQUAL(f.audioProperties()->channels(), 2);
  BOOST_CHECK_EQUAL(f.audioProperties()->sampleRate(), 44100);
}

BOOST_AUTO_TEST_CASE(testSplitPackets)
{
  const ScopedFileCopy copy("empty", ".spx");
  const String text = longText(128 * 1024, true);
  {
    Ogg::Speex::File f(copy.fileName());
    f.tag()->setTitle(text);
    f.save();
  }
  {
    Ogg::Speex::File f(copy.fileName());
    BOOST_CHECK(f.isValid());
    BOOST_CHECK_EQUAL(f.length(), 156330);
    BOOST_CHECK_EQUAL(f.lastPageHeader()->pageSequenceNumber(), 23);
    BOOST_CHECK_EQUAL(f.packet(0).size(), 80);
    BOOST_CHECK_EQUAL(f.packet(1).size(), 131116);
    BOOST_CHECK_EQUAL(f.packet(2).size(), 93);
    BOOST_CHECK_EQUAL(f.packet(3).size(), 93);
    BOOST_CHECK_EQUAL(f.tag()->title(), text);
  
    BOOST_CHECK(f.audioProperties());
    BOOST_CHECK_EQUAL(f.audioProperties()->lengthInMilliseconds(), 3685);
  
    f.tag()->setTitle("ABCDE");
    f.save();
  }
  {
    Ogg::Speex::File f(copy.fileName());
    BOOST_CHECK(f.isValid());
    BOOST_CHECK_EQUAL(f.length(), 24317);
    BOOST_CHECK_EQUAL(f.lastPageHeader()->pageSequenceNumber(), 7);
    BOOST_CHECK_EQUAL(f.packet(0).size(), 80);
    BOOST_CHECK_EQUAL(f.packet(1).size(), 49);
    BOOST_CHECK_EQUAL(f.packet(2).size(), 93);
    BOOST_CHECK_EQUAL(f.packet(3).size(), 93);
    BOOST_CHECK_EQUAL(f.tag()->title(), "ABCDE");
  
    BOOST_CHECK(f.audioProperties());
    BOOST_CHECK_EQUAL(f.audioProperties()->lengthInMilliseconds(), 3685);
  }
}

BOOST_AUTO_TEST_SUITE_END()
