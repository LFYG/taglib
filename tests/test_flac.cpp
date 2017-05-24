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

#include <flacfile.h>
#include <xiphcomment.h>
#include <id3v2tag.h>
#include <id3v1tag.h>
#include <tpropertymap.h>
#include <boost/test/unit_test.hpp>
#include "utils.h"
#include "loghelpers.h"

using namespace TagLib;

BOOST_AUTO_TEST_SUITE(TestFLAC)

BOOST_AUTO_TEST_CASE(testSignature)
{
  FLAC::File f(TEST_FILE_PATH_C("no-tags.flac"));
  BOOST_CHECK_EQUAL(f.audioProperties()->signature().toHex(), "a1b141f766e9849ac3db1030a20a3c77");
}

BOOST_AUTO_TEST_CASE(testMultipleCommentBlocks)
{
  const ScopedFileCopy copy("multiple-vc", ".flac");
  {
    FLAC::File f(copy.fileName());
    BOOST_CHECK_EQUAL(f.tag()->artist(), "Artist 1");
    f.tag()->setArtist("The Artist");
    f.save();
  }
  {
    FLAC::File f(copy.fileName());
    BOOST_CHECK_EQUAL(f.tag()->artist(), "The Artist");
    BOOST_CHECK_EQUAL(f.find("Artist"), 69);
    BOOST_CHECK_EQUAL(f.find("Artist", 70), -1);
  }
}

BOOST_AUTO_TEST_CASE(testReadPicture)
{
  const ScopedFileCopy copy("silence-44-s", ".flac");
  
  FLAC::File f(copy.fileName());
  List<FLAC::Picture *> lst = f.pictureList();
  BOOST_CHECK_EQUAL(lst.size(), 1);
  
  FLAC::Picture *pic = lst.front();
  BOOST_CHECK_EQUAL(pic->type(), FLAC::Picture::FrontCover);
  BOOST_CHECK_EQUAL(pic->width(), 1);
  BOOST_CHECK_EQUAL(pic->height(), 1);
  BOOST_CHECK_EQUAL(pic->colorDepth(), 24);
  BOOST_CHECK_EQUAL(pic->numColors(), 0);
  BOOST_CHECK_EQUAL(pic->mimeType(), "image/png");
  BOOST_CHECK_EQUAL(pic->description(), "A pixel.");
  BOOST_CHECK_EQUAL(pic->data().size(), 150);
}

BOOST_AUTO_TEST_CASE(testAddPicture)
{
  const ScopedFileCopy copy("silence-44-s", ".flac");
  {
    FLAC::File f(copy.fileName());
    List<FLAC::Picture *> lst = f.pictureList();
    BOOST_CHECK_EQUAL(lst.size(), 1);
  
    FLAC::Picture *newpic = new FLAC::Picture();
    newpic->setType(FLAC::Picture::BackCover);
    newpic->setWidth(5);
    newpic->setHeight(6);
    newpic->setColorDepth(16);
    newpic->setNumColors(7);
    newpic->setMimeType("image/jpeg");
    newpic->setDescription("new image");
    newpic->setData("JPEG data");
    f.addPicture(newpic);
    f.save();
  }
  {
    FLAC::File f(copy.fileName());
    List<FLAC::Picture *> lst = f.pictureList();
    BOOST_CHECK_EQUAL(lst.size(), 2);
  
    FLAC::Picture *pic = lst[0];
    BOOST_CHECK_EQUAL(pic->type(), FLAC::Picture::FrontCover);
    BOOST_CHECK_EQUAL(pic->width(), 1);
    BOOST_CHECK_EQUAL(pic->height(), 1);
    BOOST_CHECK_EQUAL(pic->colorDepth(), 24);
    BOOST_CHECK_EQUAL(pic->numColors(), 0);
    BOOST_CHECK_EQUAL(pic->mimeType(), "image/png");
    BOOST_CHECK_EQUAL(pic->description(), "A pixel.");
    BOOST_CHECK_EQUAL(pic->data().size(), 150);
  
    pic = lst[1];
    BOOST_CHECK_EQUAL(pic->type(), FLAC::Picture::BackCover);
    BOOST_CHECK_EQUAL(pic->width(), 5);
    BOOST_CHECK_EQUAL(pic->height(), 6);
    BOOST_CHECK_EQUAL(pic->colorDepth(), 16);
    BOOST_CHECK_EQUAL(pic->numColors(), 7);
    BOOST_CHECK_EQUAL(pic->mimeType(), "image/jpeg");
    BOOST_CHECK_EQUAL(pic->description(), "new image");
    BOOST_CHECK_EQUAL(pic->data(), "JPEG data");
  }
}

BOOST_AUTO_TEST_CASE(testReplacePicture)
{
  const ScopedFileCopy copy("silence-44-s", ".flac");
  {
    FLAC::File f(copy.fileName());
    List<FLAC::Picture *> lst = f.pictureList();
    BOOST_CHECK_EQUAL(lst.size(), 1);
  
    FLAC::Picture *newpic = new FLAC::Picture();
    newpic->setType(FLAC::Picture::BackCover);
    newpic->setWidth(5);
    newpic->setHeight(6);
    newpic->setColorDepth(16);
    newpic->setNumColors(7);
    newpic->setMimeType("image/jpeg");
    newpic->setDescription("new image");
    newpic->setData("JPEG data");
    f.removePictures();
    f.addPicture(newpic);
    f.save();
  }
  {
    FLAC::File f(copy.fileName());
    List<FLAC::Picture *> lst = f.pictureList();
    BOOST_CHECK_EQUAL(lst.size(), 1);
  
    FLAC::Picture *pic = lst[0];
    BOOST_CHECK_EQUAL(pic->type(), FLAC::Picture::BackCover);
    BOOST_CHECK_EQUAL(pic->width(), 5);
    BOOST_CHECK_EQUAL(pic->height(), 6);
    BOOST_CHECK_EQUAL(pic->colorDepth(), 16);
    BOOST_CHECK_EQUAL(pic->numColors(), 7);
    BOOST_CHECK_EQUAL(pic->mimeType(), "image/jpeg");
    BOOST_CHECK_EQUAL(pic->description(), "new image");
    BOOST_CHECK_EQUAL(pic->data(), "JPEG data");
  }
}

BOOST_AUTO_TEST_CASE(testRemoveAllPictures)
{
  const ScopedFileCopy copy("silence-44-s", ".flac");
  {
    FLAC::File f(copy.fileName());
    List<FLAC::Picture *> lst = f.pictureList();
    BOOST_CHECK_EQUAL(lst.size(), 1);
  
    f.removePictures();
    f.save();
  }
  {
    FLAC::File f(copy.fileName());
    List<FLAC::Picture *> lst = f.pictureList();
    BOOST_CHECK_EQUAL(lst.size(), 0);
  }
}

BOOST_AUTO_TEST_CASE(testRepeatedSave1)
{
  const ScopedFileCopy copy("silence-44-s", ".flac");
  {
    FLAC::File f(copy.fileName());
    BOOST_CHECK_EQUAL(f.tag()->title(), "Silence");
    f.tag()->setTitle("NEW TITLE");
    f.save();
    BOOST_CHECK_EQUAL(f.tag()->title(), "NEW TITLE");
    f.tag()->setTitle("NEW TITLE 2");
    f.save();
    BOOST_CHECK_EQUAL(f.tag()->title(), "NEW TITLE 2");
  }
  {
    FLAC::File f(copy.fileName());
    BOOST_CHECK_EQUAL(f.tag()->title(), "NEW TITLE 2");
  }
}

BOOST_AUTO_TEST_CASE(testRepeatedSave2)
{
  const ScopedFileCopy copy("no-tags", ".flac");
  
  FLAC::File f(copy.fileName());
  f.ID3v2Tag(true)->setTitle("0123456789");
  f.save();
  BOOST_CHECK_EQUAL(f.length(), 5735);
  f.save();
  BOOST_CHECK_EQUAL(f.length(), 5735);
  BOOST_CHECK(f.find("fLaC") >= 0);
}

BOOST_AUTO_TEST_CASE(testRepeatedSave3)
{
  const ScopedFileCopy copy("no-tags", ".flac");

  FLAC::File f(copy.fileName());
  f.xiphComment()->setTitle(longText(8 * 1024));
  f.save();
  BOOST_CHECK_EQUAL(f.length(), 12862);
  f.save();
  BOOST_CHECK_EQUAL(f.length(), 12862);
}

BOOST_AUTO_TEST_CASE(testSaveMultipleValues)
{
  const ScopedFileCopy copy("silence-44-s", ".flac");
  {
    FLAC::File f(copy.fileName());
    f.xiphComment(true)->addField("ARTIST", "artist 1", true);
    f.xiphComment(true)->addField("ARTIST", "artist 2", false);
    f.save();
  }
  {
    FLAC::File f(copy.fileName());
    const Ogg::FieldListMap m = f.xiphComment()->fieldListMap();
    BOOST_CHECK_EQUAL(m["ARTIST"].size(), 2);
    BOOST_CHECK_EQUAL(m["ARTIST"][0], "artist 1");
    BOOST_CHECK_EQUAL(m["ARTIST"][1], "artist 2");
  }
}

BOOST_AUTO_TEST_CASE(testDict)
{
  // test unicode & multiple values with dict interface
  const ScopedFileCopy copy("silence-44-s", ".flac");
  {
    FLAC::File f(copy.fileName());
    PropertyMap dict;
    dict["ARTIST"].append("artøst 1");
    dict["ARTIST"].append("artöst 2");
    f.setProperties(dict);
    f.save();
  }
  {
    FLAC::File f(copy.fileName());
    const PropertyMap dict = f.properties();
    BOOST_CHECK_EQUAL(dict["ARTIST"].size(), 2);
    BOOST_CHECK_EQUAL(dict["ARTIST"][0], "artøst 1");
    BOOST_CHECK_EQUAL(dict["ARTIST"][1], "artöst 2");
  }
}

BOOST_AUTO_TEST_CASE(testInvalid)
{
  const ScopedFileCopy copy("silence-44-s", ".flac");
  PropertyMap map;
  map[L"H\x00c4\x00d6"] = String("bla");
  FLAC::File f(copy.fileName());
  const PropertyMap invalid = f.setProperties(map);
  BOOST_CHECK_EQUAL(invalid.size(), 1);
  BOOST_CHECK(f.properties().isEmpty());
}

BOOST_AUTO_TEST_CASE(testAudioProperties)
{
  FLAC::File f(TEST_FILE_PATH_C("sinewave.flac"));
  BOOST_CHECK(f.audioProperties());
  BOOST_CHECK_EQUAL(f.audioProperties()->length(), 3);
  BOOST_CHECK_EQUAL(f.audioProperties()->lengthInSeconds(), 3);
  BOOST_CHECK_EQUAL(f.audioProperties()->lengthInMilliseconds(), 3550);
  BOOST_CHECK_EQUAL(f.audioProperties()->bitrate(), 145);
  BOOST_CHECK_EQUAL(f.audioProperties()->sampleRate(), 44100);
  BOOST_CHECK_EQUAL(f.audioProperties()->channels(), 2);
  BOOST_CHECK_EQUAL(f.audioProperties()->bitsPerSample(), 16);
  BOOST_CHECK_EQUAL(f.audioProperties()->sampleWidth(), 16);
  BOOST_CHECK_EQUAL(f.audioProperties()->sampleFrames(), 156556);
  BOOST_CHECK_EQUAL(
    f.audioProperties()->signature(), 
    "\xcf\xe3\xd9\xda\xba\xde\xab\x2c\xbf\x2c\xa2\x35\x27\x4b\x7f\x76");
}

BOOST_AUTO_TEST_CASE(testZeroSizedPadding1)
{
  const ScopedFileCopy copy("zero-sized-padding", ".flac");
  
  FLAC::File f(copy.fileName());
  BOOST_CHECK(f.isValid());
}

BOOST_AUTO_TEST_CASE(testZeroSizedPadding2)
{
  const ScopedFileCopy copy("silence-44-s", ".flac");
  {
    FLAC::File f(copy.fileName());
    f.xiphComment()->setTitle("ABC");
    f.save();
  }
  {
    FLAC::File f(copy.fileName());
    f.xiphComment()->setTitle(std::string(3067, 'X').c_str());
    f.save();
  }
  {
    FLAC::File f(copy.fileName());
    BOOST_CHECK(f.isValid());
  }
}

BOOST_AUTO_TEST_CASE(testShrinkPadding)
{
  const ScopedFileCopy copy("no-tags", ".flac");
  
  {
    FLAC::File f(copy.fileName());
    f.xiphComment()->setTitle(longText(128 * 1024));
    f.save();
    BOOST_CHECK(f.length() > 128 * 1024);
  }
  {
    FLAC::File f(copy.fileName());
    f.xiphComment()->setTitle("0123456789");
    f.save();
    BOOST_CHECK(f.length() < 8 * 1024);
  }
}

BOOST_AUTO_TEST_CASE(testSaveID3v1)
{
  const ScopedFileCopy copy("no-tags", ".flac");
  ByteVector audioStream;
  {
    FLAC::File f(copy.fileName());
    BOOST_CHECK(!f.hasID3v1Tag());
    BOOST_CHECK_EQUAL(f.length(), 4692);
  
    f.seek(0x0100);
    audioStream = f.readBlock(4436);
  
    f.ID3v1Tag(true)->setTitle("01234 56789 ABCDE FGHIJ");
    f.save();
    BOOST_CHECK(f.hasID3v1Tag());
    BOOST_CHECK_EQUAL(f.length(), 4820);
  
    f.seek(0x0100);
    BOOST_CHECK_EQUAL(f.readBlock(4436), audioStream);
  }
}

BOOST_AUTO_TEST_CASE(testUpdateID3v2)
{
  const ScopedFileCopy copy("no-tags", ".flac");
  {
    FLAC::File f(copy.fileName());
    f.ID3v2Tag(true)->setTitle("0123456789");
    f.save();
  }
  {
    FLAC::File f(copy.fileName());
    f.ID3v2Tag()->setTitle("ABCDEFGHIJ");
    f.save();
  }
  {
    FLAC::File f(copy.fileName());
    BOOST_CHECK_EQUAL(f.ID3v2Tag()->title(), "ABCDEFGHIJ");
  }
}

BOOST_AUTO_TEST_CASE(testEmptyID3v2)
{
  const ScopedFileCopy copy("no-tags", ".flac");
  {
    FLAC::File f(copy.fileName());
    f.ID3v2Tag(true);
    f.save();
  }
  {
    FLAC::File f(copy.fileName());
    BOOST_CHECK(!f.hasID3v2Tag());
  }
}

BOOST_AUTO_TEST_CASE(testStripTags)
{
  const ScopedFileCopy copy("silence-44-s", ".flac");
  {
    FLAC::File f(copy.fileName());
    f.xiphComment(true)->setTitle("XiphComment Title");
    f.ID3v1Tag(true)->setTitle("ID3v1 Title");
    f.ID3v2Tag(true)->setTitle("ID3v2 Title");
    f.save();
  }
  {
    FLAC::File f(copy.fileName());
    BOOST_CHECK(f.hasXiphComment());
    BOOST_CHECK(f.hasID3v1Tag());
    BOOST_CHECK(f.hasID3v2Tag());
    BOOST_CHECK_EQUAL(f.xiphComment()->title(), "XiphComment Title");
    BOOST_CHECK_EQUAL(f.ID3v1Tag()->title(), "ID3v1 Title");
    BOOST_CHECK_EQUAL(f.ID3v2Tag()->title(), "ID3v2 Title");
    f.strip(FLAC::File::ID3v2);
    f.save();
  }
  {
    FLAC::File f(copy.fileName());
    BOOST_CHECK(f.hasXiphComment());
    BOOST_CHECK(f.hasID3v1Tag());
    BOOST_CHECK(!f.hasID3v2Tag());
    BOOST_CHECK_EQUAL(f.xiphComment()->title(), "XiphComment Title");
    BOOST_CHECK_EQUAL(f.ID3v1Tag()->title(), "ID3v1 Title");
    f.strip(FLAC::File::ID3v1);
    f.save();
  }
  {
    FLAC::File f(copy.fileName());
    BOOST_CHECK(f.hasXiphComment());
    BOOST_CHECK(!f.hasID3v1Tag());
    BOOST_CHECK(!f.hasID3v2Tag());
    BOOST_CHECK_EQUAL(f.xiphComment()->title(), "XiphComment Title");
    f.strip(FLAC::File::XiphComment);
    f.save();
  }
  {
    FLAC::File f(copy.fileName());
    BOOST_CHECK(f.hasXiphComment());
    BOOST_CHECK(!f.hasID3v1Tag());
    BOOST_CHECK(!f.hasID3v2Tag());
    BOOST_CHECK(f.xiphComment()->isEmpty());
    BOOST_CHECK_EQUAL(f.xiphComment()->vendorID(), "reference libFLAC 1.1.0 20030126");
  }
}

BOOST_AUTO_TEST_CASE(testRemoveXiphField)
{
  const ScopedFileCopy copy("no-tags", ".flac");
  {
    FLAC::File f(copy.fileName());
    f.xiphComment(true)->setTitle("XiphComment Title");
    f.ID3v2Tag(true)->setTitle("ID3v2 Title");
    f.save();
  }
  {
    FLAC::File f(copy.fileName());
    BOOST_CHECK_EQUAL(f.xiphComment()->title(), "XiphComment Title");
    f.xiphComment()->removeFields("TITLE");
    f.save();
  }
  {
    FLAC::File f(copy.fileName());
    BOOST_CHECK(f.xiphComment()->title().isEmpty());
  }
}

BOOST_AUTO_TEST_CASE(testEmptySeekTable)
{
  const ScopedFileCopy copy("empty-seektable", ".flac");
  {
    FLAC::File f(copy.fileName());
    BOOST_CHECK(f.isValid());
    f.xiphComment(true)->setTitle("XiphComment Title");
    f.save();
  }
  {
    FLAC::File f(copy.fileName());
    BOOST_CHECK(f.isValid());
    f.seek(42);
    const ByteVector data = f.readBlock(4);
    BOOST_CHECK_EQUAL(data, ByteVector("\x03\x00\x00\x00", 4));
  }
}

BOOST_AUTO_TEST_SUITE_END()
