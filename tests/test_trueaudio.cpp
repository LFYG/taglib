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

#include <trueaudiofile.h>
#include <id3v2tag.h>
#include <id3v1tag.h>
#include <tpropertymap.h>
#include <boost/test/unit_test.hpp>
#include "utils.h"
#include "loghelpers.h"

using namespace TagLib;

BOOST_AUTO_TEST_SUITE(TestTrueAudio)

BOOST_AUTO_TEST_CASE(testReadPropertiesWithoutID3v2)
{
  TrueAudio::File f(TEST_FILE_PATH_C("empty.tta"));
  BOOST_CHECK(f.audioProperties());
  BOOST_CHECK_EQUAL(f.audioProperties()->length(), 3);
  BOOST_CHECK_EQUAL(f.audioProperties()->lengthInSeconds(), 3);
  BOOST_CHECK_EQUAL(f.audioProperties()->lengthInMilliseconds(), 3685);
  BOOST_CHECK_EQUAL(f.audioProperties()->bitrate(), 173);
  BOOST_CHECK_EQUAL(f.audioProperties()->channels(), 2);
  BOOST_CHECK_EQUAL(f.audioProperties()->sampleRate(), 44100);
  BOOST_CHECK_EQUAL(f.audioProperties()->bitsPerSample(), 16);
  BOOST_CHECK_EQUAL(f.audioProperties()->sampleFrames(), 162496);
  BOOST_CHECK_EQUAL(f.audioProperties()->ttaVersion(), 1);
}

BOOST_AUTO_TEST_CASE(testReadPropertiesWithTags)
{
  TrueAudio::File f(TEST_FILE_PATH_C("tagged.tta"));
  BOOST_CHECK(f.audioProperties());
  BOOST_CHECK_EQUAL(f.audioProperties()->length(), 3);
  BOOST_CHECK_EQUAL(f.audioProperties()->lengthInSeconds(), 3);
  BOOST_CHECK_EQUAL(f.audioProperties()->lengthInMilliseconds(), 3685);
  BOOST_CHECK_EQUAL(f.audioProperties()->bitrate(), 173);
  BOOST_CHECK_EQUAL(f.audioProperties()->channels(), 2);
  BOOST_CHECK_EQUAL(f.audioProperties()->sampleRate(), 44100);
  BOOST_CHECK_EQUAL(f.audioProperties()->bitsPerSample(), 16);
  BOOST_CHECK_EQUAL(f.audioProperties()->sampleFrames(), 162496);
  BOOST_CHECK_EQUAL(f.audioProperties()->ttaVersion(), 1);
}

BOOST_AUTO_TEST_CASE(testStripAndProperties)
{
  const ScopedFileCopy copy("empty", ".tta");
  {
    TrueAudio::File f(copy.fileName());
    f.ID3v2Tag(true)->setTitle("ID3v2");
    f.ID3v1Tag(true)->setTitle("ID3v1");
    f.save();
  }
  {
    TrueAudio::File f(copy.fileName());
    BOOST_CHECK_EQUAL(f.properties()["TITLE"].front(), "ID3v2");
    f.strip(TrueAudio::File::ID3v2);
    BOOST_CHECK_EQUAL(f.properties()["TITLE"].front(), "ID3v1");
    f.strip(TrueAudio::File::ID3v1);
    BOOST_CHECK(f.properties().isEmpty());
  }
}

BOOST_AUTO_TEST_CASE(testRepeatedSave)
{
  const ScopedFileCopy copy("empty", ".tta");
  {
    TrueAudio::File f(copy.fileName());
    BOOST_CHECK(!f.hasID3v2Tag());
    BOOST_CHECK(!f.hasID3v1Tag());
  
    f.ID3v2Tag(true)->setTitle("01234 56789 ABCDE FGHIJ");
    f.save();
  
    f.ID3v2Tag()->setTitle("0");
    f.save();
  
    f.ID3v1Tag(true)->setTitle("01234 56789 ABCDE FGHIJ");
    f.ID3v2Tag()->setTitle("01234 56789 ABCDE FGHIJ 01234 56789 ABCDE FGHIJ 01234 56789");
    f.save();
  }
  {
    TrueAudio::File f(copy.fileName());
    BOOST_CHECK(f.hasID3v2Tag());
    BOOST_CHECK(f.hasID3v1Tag());
  }
}

BOOST_AUTO_TEST_SUITE_END()
