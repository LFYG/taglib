/***************************************************************************
    copyright           : (C) 2010 by Lukas Lalinsky
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

#include <wavpackfile.h>
#include <apetag.h>
#include <id3v1tag.h>
#include <tpropertymap.h>
#include <boost/test/unit_test.hpp>
#include "utils.h"
#include "loghelpers.h"

using namespace TagLib;

BOOST_AUTO_TEST_SUITE(TestWavPack)

BOOST_AUTO_TEST_CASE(testNoLengthProperties)
{
  WavPack::File f(TEST_FILE_PATH_C("no_length.wv"));
  BOOST_CHECK(f.audioProperties());
  BOOST_CHECK_EQUAL(f.audioProperties()->length(), 3);
  BOOST_CHECK_EQUAL(f.audioProperties()->lengthInSeconds(), 3);
  BOOST_CHECK_EQUAL(f.audioProperties()->lengthInMilliseconds(), 3705);
  BOOST_CHECK_EQUAL(f.audioProperties()->bitrate(), 1);
  BOOST_CHECK_EQUAL(f.audioProperties()->channels(), 2);
  BOOST_CHECK_EQUAL(f.audioProperties()->bitsPerSample(), 16);
  BOOST_CHECK_EQUAL(f.audioProperties()->isLossless(), true);
  BOOST_CHECK_EQUAL(f.audioProperties()->sampleRate(), 44100);
  BOOST_CHECK_EQUAL(f.audioProperties()->sampleFrames(), 163392);
  BOOST_CHECK_EQUAL(f.audioProperties()->version(), 1031);
}

BOOST_AUTO_TEST_CASE(testMultiChannelProperties)
{
  WavPack::File f(TEST_FILE_PATH_C("four_channels.wv"));
  BOOST_CHECK(f.audioProperties());
  BOOST_CHECK_EQUAL(f.audioProperties()->length(), 3);
  BOOST_CHECK_EQUAL(f.audioProperties()->lengthInSeconds(), 3);
  BOOST_CHECK_EQUAL(f.audioProperties()->lengthInMilliseconds(), 3833);
  BOOST_CHECK_EQUAL(f.audioProperties()->bitrate(), 112);
  BOOST_CHECK_EQUAL(f.audioProperties()->channels(), 4);
  BOOST_CHECK_EQUAL(f.audioProperties()->bitsPerSample(), 16);
  BOOST_CHECK_EQUAL(f.audioProperties()->isLossless(), false);
  BOOST_CHECK_EQUAL(f.audioProperties()->sampleRate(), 44100);
  BOOST_CHECK_EQUAL(f.audioProperties()->sampleFrames(), 169031);
  BOOST_CHECK_EQUAL(f.audioProperties()->version(), 1031);
}

BOOST_AUTO_TEST_CASE(testTaggedProperties)
{
  WavPack::File f(TEST_FILE_PATH_C("tagged.wv"));
  BOOST_CHECK(f.audioProperties());
  BOOST_CHECK_EQUAL(f.audioProperties()->length(), 3);
  BOOST_CHECK_EQUAL(f.audioProperties()->lengthInSeconds(), 3);
  BOOST_CHECK_EQUAL(f.audioProperties()->lengthInMilliseconds(), 3550);
  BOOST_CHECK_EQUAL(f.audioProperties()->bitrate(), 172);
  BOOST_CHECK_EQUAL(f.audioProperties()->channels(), 2);
  BOOST_CHECK_EQUAL(f.audioProperties()->bitsPerSample(), 16);
  BOOST_CHECK_EQUAL(f.audioProperties()->isLossless(), false);
  BOOST_CHECK_EQUAL(f.audioProperties()->sampleRate(), 44100);
  BOOST_CHECK_EQUAL(f.audioProperties()->sampleFrames(), 156556);
  BOOST_CHECK_EQUAL(f.audioProperties()->version(), 1031);
}

BOOST_AUTO_TEST_CASE(testFuzzedFile)
{
  WavPack::File f(TEST_FILE_PATH_C("infloop.wv"));
  BOOST_CHECK(f.isValid());
}

BOOST_AUTO_TEST_CASE(testStripAndProperties)
{
  const ScopedFileCopy copy("click", ".wv");
  {
    WavPack::File f(copy.fileName());
    f.APETag(true)->setTitle("APE");
    f.ID3v1Tag(true)->setTitle("ID3v1");
    f.save();
  }
  {
    WavPack::File f(copy.fileName());
    BOOST_CHECK_EQUAL(f.properties()["TITLE"].front(), "APE");
    f.strip(WavPack::File::APE);
    BOOST_CHECK_EQUAL(f.properties()["TITLE"].front(), "ID3v1");
    f.strip(WavPack::File::ID3v1);
    BOOST_CHECK(f.properties().isEmpty());
  }
}

BOOST_AUTO_TEST_CASE(testRepeatedSave)
{
  const ScopedFileCopy copy("click", ".wv");
  {
    WavPack::File f(copy.fileName());
    BOOST_CHECK(!f.hasAPETag());
    BOOST_CHECK(!f.hasID3v1Tag());
  
    f.APETag(true)->setTitle("01234 56789 ABCDE FGHIJ");
    f.save();
  
    f.APETag()->setTitle("0");
    f.save();
  
    f.ID3v1Tag(true)->setTitle("01234 56789 ABCDE FGHIJ");
    f.APETag()->setTitle("01234 56789 ABCDE FGHIJ 01234 56789 ABCDE FGHIJ 01234 56789");
    f.save();
  }
  {
    WavPack::File f(copy.fileName());
    BOOST_CHECK(f.hasAPETag());
    BOOST_CHECK(f.hasID3v1Tag());
  }
}

BOOST_AUTO_TEST_SUITE_END()
