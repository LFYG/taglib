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

#include <mpegfile.h>
#include <id3v2tag.h>
#include <id3v1tag.h>
#include <apetag.h>
#include <mpegproperties.h>
#include <mpegheader.h>
#include <xingheader.h>
#include <tpropertymap.h>
#include <boost/test/unit_test.hpp>
#include "utils.h"
#include "loghelpers.h"

using namespace TagLib;

BOOST_AUTO_TEST_SUITE(TestMPEG)

BOOST_AUTO_TEST_CASE(testAudioPropertiesXingHeaderCBR)
{
  MPEG::File f(TEST_FILE_PATH_C("lame_cbr.mp3"));
  BOOST_CHECK(f.audioProperties());
  BOOST_CHECK_EQUAL(f.audioProperties()->length(), 1887);
  BOOST_CHECK_EQUAL(f.audioProperties()->lengthInSeconds(), 1887);
  BOOST_CHECK_EQUAL(f.audioProperties()->lengthInMilliseconds(), 1887164);
  BOOST_CHECK_EQUAL(f.audioProperties()->bitrate(), 64);
  BOOST_CHECK_EQUAL(f.audioProperties()->channels(), 1);
  BOOST_CHECK_EQUAL(f.audioProperties()->sampleRate(), 44100);
  BOOST_CHECK_EQUAL(f.audioProperties()->xingHeader()->type(), MPEG::XingHeader::Xing);
}

BOOST_AUTO_TEST_CASE(testAudioPropertiesXingHeaderVBR)
{
  MPEG::File f(TEST_FILE_PATH_C("lame_vbr.mp3"));
  BOOST_CHECK(f.audioProperties());
  BOOST_CHECK_EQUAL(f.audioProperties()->length(), 1887);
  BOOST_CHECK_EQUAL(f.audioProperties()->lengthInSeconds(), 1887);
  BOOST_CHECK_EQUAL(f.audioProperties()->lengthInMilliseconds(), 1887164);
  BOOST_CHECK_EQUAL(f.audioProperties()->bitrate(), 70);
  BOOST_CHECK_EQUAL(f.audioProperties()->channels(), 1);
  BOOST_CHECK_EQUAL(f.audioProperties()->sampleRate(), 44100);
  BOOST_CHECK_EQUAL(f.audioProperties()->xingHeader()->type(), MPEG::XingHeader::Xing);
}

BOOST_AUTO_TEST_CASE(testAudioPropertiesVBRIHeader)
{
  MPEG::File f(TEST_FILE_PATH_C("rare_frames.mp3"));
  BOOST_CHECK(f.audioProperties());
  BOOST_CHECK_EQUAL(f.audioProperties()->length(), 222);
  BOOST_CHECK_EQUAL(f.audioProperties()->lengthInSeconds(), 222);
  BOOST_CHECK_EQUAL(f.audioProperties()->lengthInMilliseconds(), 222198);
  BOOST_CHECK_EQUAL(f.audioProperties()->bitrate(), 233);
  BOOST_CHECK_EQUAL(f.audioProperties()->channels(), 2);
  BOOST_CHECK_EQUAL(f.audioProperties()->sampleRate(), 44100);
  BOOST_CHECK_EQUAL(f.audioProperties()->xingHeader()->type(), MPEG::XingHeader::VBRI);
}

BOOST_AUTO_TEST_CASE(testAudioPropertiesNoVBRHeaders)
{
  MPEG::File f(TEST_FILE_PATH_C("bladeenc.mp3"));
  BOOST_CHECK(f.audioProperties());
  BOOST_CHECK_EQUAL(f.audioProperties()->length(), 3);
  BOOST_CHECK_EQUAL(f.audioProperties()->lengthInSeconds(), 3);
  BOOST_CHECK_EQUAL(f.audioProperties()->lengthInMilliseconds(), 3553);
  BOOST_CHECK_EQUAL(f.audioProperties()->bitrate(), 64);
  BOOST_CHECK_EQUAL(f.audioProperties()->channels(), 1);
  BOOST_CHECK_EQUAL(f.audioProperties()->sampleRate(), 44100);
  BOOST_CHECK(!f.audioProperties()->xingHeader());
  
  const long last = f.lastFrameOffset();
  const MPEG::Header lastHeader(&f, last, false);
  
  BOOST_CHECK_EQUAL(last, 28213);
  BOOST_CHECK_EQUAL(lastHeader.frameLength(), 209);
}

BOOST_AUTO_TEST_CASE(testSkipInvalidFrames1)
{
  MPEG::File f(TEST_FILE_PATH_C("invalid-frames1.mp3"));
  BOOST_CHECK(f.audioProperties());
  BOOST_CHECK_EQUAL(f.audioProperties()->length(), 0);
  BOOST_CHECK_EQUAL(f.audioProperties()->lengthInSeconds(), 0);
  BOOST_CHECK_EQUAL(f.audioProperties()->lengthInMilliseconds(), 392);
  BOOST_CHECK_EQUAL(f.audioProperties()->bitrate(), 160);
  BOOST_CHECK_EQUAL(f.audioProperties()->channels(), 2);
  BOOST_CHECK_EQUAL(f.audioProperties()->sampleRate(), 44100);
  BOOST_CHECK(!f.audioProperties()->xingHeader());
}

BOOST_AUTO_TEST_CASE(testSkipInvalidFrames2)
{
  MPEG::File f(TEST_FILE_PATH_C("invalid-frames2.mp3"));
  BOOST_CHECK(f.audioProperties());
  BOOST_CHECK_EQUAL(f.audioProperties()->length(), 0);
  BOOST_CHECK_EQUAL(f.audioProperties()->lengthInSeconds(), 0);
  BOOST_CHECK_EQUAL(f.audioProperties()->lengthInMilliseconds(), 314);
  BOOST_CHECK_EQUAL(f.audioProperties()->bitrate(), 192);
  BOOST_CHECK_EQUAL(f.audioProperties()->channels(), 2);
  BOOST_CHECK_EQUAL(f.audioProperties()->sampleRate(), 44100);
  BOOST_CHECK(!f.audioProperties()->xingHeader());
}

BOOST_AUTO_TEST_CASE(testSkipInvalidFrames3)
{
  MPEG::File f(TEST_FILE_PATH_C("invalid-frames3.mp3"));
  BOOST_CHECK(f.audioProperties());
  BOOST_CHECK_EQUAL(f.audioProperties()->length(), 0);
  BOOST_CHECK_EQUAL(f.audioProperties()->lengthInSeconds(), 0);
  BOOST_CHECK_EQUAL(f.audioProperties()->lengthInMilliseconds(), 183);
  BOOST_CHECK_EQUAL(f.audioProperties()->bitrate(), 320);
  BOOST_CHECK_EQUAL(f.audioProperties()->channels(), 2);
  BOOST_CHECK_EQUAL(f.audioProperties()->sampleRate(), 44100);
  BOOST_CHECK(!f.audioProperties()->xingHeader());
}

BOOST_AUTO_TEST_CASE(testVersion2DurationWithXingHeader)
{
  MPEG::File f(TEST_FILE_PATH_C("mpeg2.mp3"));
  BOOST_CHECK(f.audioProperties());
  BOOST_CHECK_EQUAL(f.audioProperties()->length(), 5387);
  BOOST_CHECK_EQUAL(f.audioProperties()->lengthInSeconds(), 5387);
  BOOST_CHECK_EQUAL(f.audioProperties()->lengthInMilliseconds(), 5387285);
}

BOOST_AUTO_TEST_CASE(testSaveID3v24)
{
  const ScopedFileCopy copy("xing", ".mp3");
  const String xxx(ByteVector(254, 'X'));
  {
    MPEG::File f(copy.fileName());
    BOOST_CHECK_EQUAL(f.hasID3v2Tag(), false);
  
    f.tag()->setTitle(xxx);
    f.tag()->setArtist("Artist A");
    f.save(MPEG::File::AllTags, true, 4);
    BOOST_CHECK_EQUAL(f.hasID3v2Tag(), true);
  }
  {
    MPEG::File f(copy.fileName());
    BOOST_CHECK_EQUAL(f.ID3v2Tag()->header()->majorVersion(), 4);
    BOOST_CHECK_EQUAL(f.tag()->artist(), "Artist A");
    BOOST_CHECK_EQUAL(f.tag()->title(), xxx);
  }
}

BOOST_AUTO_TEST_CASE(testSaveID3v24WrongParam)
{
  const ScopedFileCopy copy("xing", ".mp3");
  const String xxx(ByteVector(254, 'X'));
  {
    MPEG::File f(copy.fileName());
    f.tag()->setTitle(xxx);
    f.tag()->setArtist("Artist A");
    f.save(MPEG::File::AllTags, true, 8);
  }
  {
    MPEG::File f(copy.fileName());
    BOOST_CHECK_EQUAL(f.ID3v2Tag()->header()->majorVersion(), 4);
    BOOST_CHECK_EQUAL(f.tag()->artist(), "Artist A");
    BOOST_CHECK_EQUAL(f.tag()->title(), xxx);
  }
}

BOOST_AUTO_TEST_CASE(testSaveID3v23)
{
  const ScopedFileCopy copy("xing", ".mp3");
  const String xxx(ByteVector(254, 'X'));
  {
    MPEG::File f(copy.fileName());
    BOOST_CHECK_EQUAL(f.hasID3v2Tag(), false);
  
    f.tag()->setTitle(xxx);
    f.tag()->setArtist("Artist A");
    f.save(MPEG::File::AllTags, true, 3);
    BOOST_CHECK_EQUAL(f.hasID3v2Tag(), true);
  }
  {
    MPEG::File f(copy.fileName());
    BOOST_CHECK_EQUAL(f.ID3v2Tag()->header()->majorVersion(), 3);
    BOOST_CHECK_EQUAL(f.tag()->artist(), "Artist A");
    BOOST_CHECK_EQUAL(f.tag()->title(), xxx);
  }
}

BOOST_AUTO_TEST_CASE(testDuplicateID3v2)
{
  MPEG::File f(TEST_FILE_PATH_C("duplicate_id3v2.mp3"));
  
  // duplicate_id3v2.mp3 has duplicate ID3v2 tags.
  // Sample rate will be 32000 if can't skip the second tag.
  
  BOOST_CHECK(f.hasID3v2Tag());
  BOOST_CHECK_EQUAL(f.audioProperties()->sampleRate(), 44100);
}

BOOST_AUTO_TEST_CASE(testFuzzedFile)
{
  MPEG::File f(TEST_FILE_PATH_C("excessive_alloc.mp3"));
  BOOST_CHECK(f.isValid());
}

BOOST_AUTO_TEST_CASE(testFrameOffset)
{
  {
    MPEG::File f(TEST_FILE_PATH_C("ape.mp3"));
    BOOST_CHECK(f.isValid());
    BOOST_CHECK_EQUAL(f.firstFrameOffset(), 0x0000);
    BOOST_CHECK_EQUAL(f.lastFrameOffset(), 0x1FD6);
  }
  {
    MPEG::File f(TEST_FILE_PATH_C("ape-id3v1.mp3"));
    BOOST_CHECK(f.isValid());
    BOOST_CHECK_EQUAL(f.firstFrameOffset(), 0x0000);
    BOOST_CHECK_EQUAL(f.lastFrameOffset(), 0x1FD6);
  }
  {
    MPEG::File f(TEST_FILE_PATH_C("ape-id3v2.mp3"));
    BOOST_CHECK(f.isValid());
    BOOST_CHECK_EQUAL(f.firstFrameOffset(), 0x041A);
    BOOST_CHECK_EQUAL(f.lastFrameOffset(), 0x23F0);
  }
}

BOOST_AUTO_TEST_CASE(testStripAndProperties)
{
  const ScopedFileCopy copy("xing", ".mp3");
  {
    MPEG::File f(copy.fileName());
    f.ID3v2Tag(true)->setTitle("ID3v2");
    f.APETag(true)->setTitle("APE");
    f.ID3v1Tag(true)->setTitle("ID3v1");
    f.save();
  }
  {
    MPEG::File f(copy.fileName());
    BOOST_CHECK_EQUAL(f.properties()["TITLE"].front(), "ID3v2");
    f.strip(MPEG::File::ID3v2);
    BOOST_CHECK_EQUAL(f.properties()["TITLE"].front(), "APE");
    f.strip(MPEG::File::APE);
    BOOST_CHECK_EQUAL(f.properties()["TITLE"].front(), "ID3v1");
    f.strip(MPEG::File::ID3v1);
    BOOST_CHECK(f.properties().isEmpty());
  }
}

BOOST_AUTO_TEST_CASE(testRepeatedSave1)
{
  const ScopedFileCopy copy("xing", ".mp3");
  {
    MPEG::File f(copy.fileName());
    f.ID3v2Tag(true)->setTitle(std::string(4096, 'X').c_str());
    f.save();
  }
  {
    MPEG::File f(copy.fileName());
    f.ID3v2Tag(true)->setTitle("");
    f.save();
    f.ID3v2Tag(true)->setTitle(std::string(4096, 'X').c_str());
    f.save();
    BOOST_CHECK_EQUAL(f.firstFrameOffset(), 5141);
  }
}

BOOST_AUTO_TEST_CASE(testRepeatedSave2)
{
  const ScopedFileCopy copy("xing", ".mp3");

  MPEG::File f(copy.fileName());
  f.ID3v2Tag(true)->setTitle("0123456789");
  f.save();
  f.save();
  BOOST_CHECK_EQUAL(f.find("ID3", 3), -1);
}

BOOST_AUTO_TEST_CASE(testRepeatedSave3)
{
  const ScopedFileCopy copy("xing", ".mp3");
  {
    MPEG::File f(copy.fileName());
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
    MPEG::File f(copy.fileName());
    BOOST_CHECK(f.hasAPETag());
    BOOST_CHECK(f.hasID3v1Tag());
  }
}

BOOST_AUTO_TEST_CASE(testEmptyID3v2)
{
  const ScopedFileCopy copy("xing", ".mp3");
  {
    MPEG::File f(copy.fileName());
    f.ID3v2Tag(true)->setTitle("0123456789");
    f.save(MPEG::File::ID3v2);
  }
  {
    MPEG::File f(copy.fileName());
    f.ID3v2Tag(true)->setTitle("");
    f.save(MPEG::File::ID3v2, false);
  }
  {
    MPEG::File f(copy.fileName());
    BOOST_CHECK(!f.hasID3v2Tag());
  }
}

BOOST_AUTO_TEST_CASE(testEmptyID3v1)
{
  const ScopedFileCopy copy("xing", ".mp3");
  {
    MPEG::File f(copy.fileName());
    f.ID3v1Tag(true)->setTitle("0123456789");
    f.save(MPEG::File::ID3v1);
  }
  {
    MPEG::File f(copy.fileName());
    f.ID3v1Tag(true)->setTitle("");
    f.save(MPEG::File::ID3v1, false);
  }
  {
    MPEG::File f(copy.fileName());
    BOOST_CHECK(!f.hasID3v1Tag());
  }
}

BOOST_AUTO_TEST_CASE(testEmptyAPE)
{
  const ScopedFileCopy copy("xing", ".mp3");
  {
    MPEG::File f(copy.fileName());
    f.APETag(true)->setTitle("0123456789");
    f.save(MPEG::File::APE);
  }
  {
    MPEG::File f(copy.fileName());
    f.APETag(true)->setTitle("");
    f.save(MPEG::File::APE, false);
  }
  {
    MPEG::File f(copy.fileName());
    BOOST_CHECK(!f.hasAPETag());
  }
}

BOOST_AUTO_TEST_CASE(testIgnoreGarbage)
{
  const ScopedFileCopy copy("garbage", ".mp3");
  {
    MPEG::File f(copy.fileName());
    BOOST_CHECK(f.isValid());
    BOOST_CHECK(f.hasID3v2Tag());
    BOOST_CHECK_EQUAL(f.firstFrameOffset(), 2255);
    BOOST_CHECK_EQUAL(f.lastFrameOffset(), 6015);
    BOOST_CHECK_EQUAL(f.ID3v2Tag()->title(), "Title A");
    f.ID3v2Tag()->setTitle("Title B");
    f.save();
  }
  {
    MPEG::File f(copy.fileName());
    BOOST_CHECK(f.isValid());
    BOOST_CHECK(f.hasID3v2Tag());
    BOOST_CHECK_EQUAL(f.ID3v2Tag()->title(), "Title B");
  }
}

BOOST_AUTO_TEST_SUITE_END()
