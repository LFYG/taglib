/***************************************************************************
    copyright           : (C) 2008 by Lukas Lalinsky
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

#include <asffile.h>
#include <tpropertymap.h>
#include <boost/test/unit_test.hpp>
#include "utils.h"
#include "loghelpers.h"

using namespace TagLib;

BOOST_AUTO_TEST_SUITE(TestASF)

BOOST_AUTO_TEST_CASE(testAudioProperties)
{
  ASF::File f(TEST_FILE_PATH_C("silence-1.wma"));
  BOOST_CHECK(f.audioProperties());
  BOOST_CHECK_EQUAL(f.audioProperties()->length(), 3);
  BOOST_CHECK_EQUAL(f.audioProperties()->lengthInSeconds(), 3);
  BOOST_CHECK_EQUAL(f.audioProperties()->lengthInMilliseconds(), 3712);
  BOOST_CHECK_EQUAL(f.audioProperties()->bitrate(), 64);
  BOOST_CHECK_EQUAL(f.audioProperties()->channels(), 2);
  BOOST_CHECK_EQUAL(f.audioProperties()->sampleRate(), 48000);
  BOOST_CHECK_EQUAL(f.audioProperties()->bitsPerSample(), 16);
  BOOST_CHECK_EQUAL(f.audioProperties()->codec(), ASF::Properties::WMA2);
  BOOST_CHECK_EQUAL(f.audioProperties()->codecName(), "Windows Media Audio 9.1");
  BOOST_CHECK_EQUAL(f.audioProperties()->codecDescription(), "64 kbps, 48 kHz, stereo 2-pass CBR");
  BOOST_CHECK(!f.audioProperties()->isEncrypted());
}

BOOST_AUTO_TEST_CASE(testLosslessProperties)
{
  ASF::File f(TEST_FILE_PATH_C("lossless.wma"));
  BOOST_CHECK(f.audioProperties());
  BOOST_CHECK_EQUAL(f.audioProperties()->length(), 3);
  BOOST_CHECK_EQUAL(f.audioProperties()->lengthInSeconds(), 3);
  BOOST_CHECK_EQUAL(f.audioProperties()->lengthInMilliseconds(), 3549);
  BOOST_CHECK_EQUAL(f.audioProperties()->bitrate(), 1152);
  BOOST_CHECK_EQUAL(f.audioProperties()->channels(), 2);
  BOOST_CHECK_EQUAL(f.audioProperties()->sampleRate(), 44100);
  BOOST_CHECK_EQUAL(f.audioProperties()->bitsPerSample(), 16);
  BOOST_CHECK_EQUAL(f.audioProperties()->codec(), ASF::Properties::WMA9Lossless);
  BOOST_CHECK_EQUAL(f.audioProperties()->codecName(), "Windows Media Audio 9.2 Lossless");
  BOOST_CHECK_EQUAL(f.audioProperties()->codecDescription(), "VBR Quality 100, 44 kHz, 2 channel 16 bit 1-pass VBR");
  BOOST_CHECK(!f.audioProperties()->isEncrypted());
}

BOOST_AUTO_TEST_CASE(testRead)
{
  ASF::File f(TEST_FILE_PATH_C("silence-1.wma"));
  BOOST_CHECK_EQUAL(f.tag()->title(), "test");
}

BOOST_AUTO_TEST_CASE(testSaveMultipleValues)
{
  const ScopedFileCopy copy("silence-1", ".wma");
  {
    ASF::File f(copy.fileName());
    ASF::AttributeList values;
    values.append("Foo");
    values.append("Bar");
    f.tag()->setAttribute("WM/AlbumTitle", values);
    f.save();
  }
  {
    ASF::File f(copy.fileName());
    BOOST_CHECK_EQUAL(f.tag()->attributeListMap()["WM/AlbumTitle"].size(), 2);
  }
}

BOOST_AUTO_TEST_CASE(testSaveStream)
{
  const ScopedFileCopy copy("silence-1", ".wma");
  {
    ASF::File f(copy.fileName());
    ASF::Attribute attr("Foo");
    attr.setStream(43);
    f.tag()->setAttribute("WM/AlbumTitle", attr);
    f.save();
  }
  {
    ASF::File f(copy.fileName());
    BOOST_CHECK_EQUAL(f.tag()->attribute("WM/AlbumTitle").front().stream(), 43);
  }
}

BOOST_AUTO_TEST_CASE(testSaveLanguage)
{
  const ScopedFileCopy copy("silence-1", ".wma");
  {
    ASF::File f(copy.fileName());
    ASF::Attribute attr("Foo");
    attr.setStream(32);
    attr.setLanguage(56);
    f.tag()->setAttribute("WM/AlbumTitle", attr);
    f.save();
  }
  {
    ASF::File f(copy.fileName());
    BOOST_CHECK_EQUAL(f.tag()->attribute("WM/AlbumTitle").front().stream(), 32);
    BOOST_CHECK_EQUAL(f.tag()->attribute("WM/AlbumTitle").front().language(), 56);
  }
}

BOOST_AUTO_TEST_CASE(testDWordTrackNumber)
{
  const ScopedFileCopy copy("silence-1", ".wma");
  {
    ASF::File f(copy.fileName());
    BOOST_CHECK(!f.tag()->contains("WM/TrackNumber"));
    f.tag()->setAttribute("WM/TrackNumber", 123U);
    f.save();
  }
  {
    ASF::File f(copy.fileName());
    BOOST_CHECK(f.tag()->contains("WM/TrackNumber"));
    BOOST_CHECK_EQUAL(f.tag()->attribute("WM/TrackNumber").front().type(), ASF::Attribute::DWordType);
    BOOST_CHECK_EQUAL(f.tag()->track(), 123);
    f.tag()->setTrack(234);
    f.save();
  }
  {
    ASF::File f(copy.fileName());
    BOOST_CHECK(f.tag()->contains("WM/TrackNumber"));
    BOOST_CHECK_EQUAL(f.tag()->attribute("WM/TrackNumber").front().type(), ASF::Attribute::UnicodeType);
    BOOST_CHECK_EQUAL(f.tag()->track(), 234);
  }
}

BOOST_AUTO_TEST_CASE(testSaveLargeValue)
{
  const ScopedFileCopy copy("silence-1", ".wma");
  {
    ASF::File f(copy.fileName());
    ASF::Attribute attr(ByteVector(70000, 'x'));
    f.tag()->setAttribute("WM/Blob", attr);
    f.save();
  }
  {
    ASF::File f(copy.fileName());
    BOOST_CHECK_EQUAL(f.tag()->attribute("WM/Blob").front().toByteVector(), ByteVector(70000, 'x'));
  }
}

BOOST_AUTO_TEST_CASE(testSavePicture)
{
  const ScopedFileCopy copy("silence-1", ".wma");
  {
    ASF::File f(copy.fileName());
    ASF::Picture picture;
    picture.setMimeType("image/jpeg");
    picture.setType(ASF::Picture::FrontCover);
    picture.setDescription("description");
    picture.setPicture("data");
    f.tag()->setAttribute("WM/Picture", picture);
    f.save();
  }
  {
    ASF::File f(copy.fileName());
    ASF::AttributeList values2 = f.tag()->attribute("WM/Picture");
    BOOST_CHECK_EQUAL(values2.size(), 1);
    ASF::Attribute attr2 = values2.front();
    ASF::Picture picture2 = attr2.toPicture();
    BOOST_CHECK(picture2.isValid());
    BOOST_CHECK_EQUAL(picture2.mimeType(), "image/jpeg");
    BOOST_CHECK_EQUAL(picture2.type(), ASF::Picture::FrontCover);
    BOOST_CHECK_EQUAL(picture2.description(), "description");
    BOOST_CHECK_EQUAL(picture2.picture(), "data");
  }
}

BOOST_AUTO_TEST_CASE(testSaveMultiplePictures)
{
  const ScopedFileCopy copy("silence-1", ".wma");
  {
    ASF::File f(copy.fileName());
    ASF::AttributeList values;
    ASF::Picture picture;
    picture.setMimeType("image/jpeg");
    picture.setType(ASF::Picture::FrontCover);
    picture.setDescription("description");
    picture.setPicture("data");
    values.append(ASF::Attribute(picture));
    ASF::Picture picture2;
    picture2.setMimeType("image/png");
    picture2.setType(ASF::Picture::BackCover);
    picture2.setDescription("back cover");
    picture2.setPicture("PNG data");
    values.append(ASF::Attribute(picture2));
    f.tag()->setAttribute("WM/Picture", values);
    f.save();
  }
  {
    ASF::File f(copy.fileName());
    ASF::AttributeList values2 = f.tag()->attribute("WM/Picture");
    BOOST_CHECK_EQUAL(values2.size(), 2);
    ASF::Picture picture3 = values2[1].toPicture();
    BOOST_CHECK(picture3.isValid());
    BOOST_CHECK_EQUAL(picture3.mimeType(), "image/jpeg");
    BOOST_CHECK_EQUAL(picture3.type(), ASF::Picture::FrontCover);
    BOOST_CHECK_EQUAL(picture3.description(), "description");
    BOOST_CHECK_EQUAL(picture3.picture(), "data");
    ASF::Picture picture4 = values2[0].toPicture();
    BOOST_CHECK(picture4.isValid());
    BOOST_CHECK_EQUAL(picture4.mimeType(), "image/png");
    BOOST_CHECK_EQUAL(picture4.type(), ASF::Picture::BackCover);
    BOOST_CHECK_EQUAL(picture4.description(), "back cover");
    BOOST_CHECK_EQUAL(picture4.picture(), "PNG data");
  }
}

BOOST_AUTO_TEST_CASE(testProperties)
{
  ASF::File f(TEST_FILE_PATH_C("silence-1.wma"));
  
  PropertyMap tags = f.properties();
  
  tags["TRACKNUMBER"] = StringList("2");
  tags["DISCNUMBER"] = StringList("3");
  tags["BPM"] = StringList("123");
  tags["ARTIST"] = StringList("Foo Bar");
  f.setProperties(tags);
  
  tags = f.properties();
  
  BOOST_CHECK_EQUAL(f.tag()->artist(), "Foo Bar");
  BOOST_CHECK_EQUAL(tags["ARTIST"], StringList("Foo Bar"));
  
  BOOST_CHECK(f.tag()->contains("WM/BeatsPerMinute"));
  BOOST_CHECK_EQUAL(f.tag()->attributeListMap()["WM/BeatsPerMinute"].size(), 1u);
  BOOST_CHECK_EQUAL(f.tag()->attribute("WM/BeatsPerMinute").front().toString(), "123");
  BOOST_CHECK_EQUAL(tags["BPM"], StringList("123"));
  
  BOOST_CHECK(f.tag()->contains("WM/TrackNumber"));
  BOOST_CHECK_EQUAL(f.tag()->attributeListMap()["WM/TrackNumber"].size(), 1u);
  BOOST_CHECK_EQUAL(f.tag()->attribute("WM/TrackNumber").front().toString(), "2");
  BOOST_CHECK_EQUAL(tags["TRACKNUMBER"], StringList("2"));
  
  BOOST_CHECK(f.tag()->contains("WM/PartOfSet"));
  BOOST_CHECK_EQUAL(f.tag()->attributeListMap()["WM/PartOfSet"].size(), 1u);
  BOOST_CHECK_EQUAL(f.tag()->attribute("WM/PartOfSet").front().toString(), "3");
  BOOST_CHECK_EQUAL(tags["DISCNUMBER"], StringList("3"));
}

BOOST_AUTO_TEST_CASE(testRepeatedSave)
{
  const ScopedFileCopy copy("silence-1", ".wma");
  {
    ASF::File f(copy.fileName());
    f.tag()->setTitle(longText(128 * 1024));
    f.save();
    BOOST_CHECK_EQUAL(f.length(), 297578);
    f.tag()->setTitle(longText(16 * 1024));
    f.save();
    BOOST_CHECK_EQUAL(f.length(), 68202);
  }
}

BOOST_AUTO_TEST_SUITE_END()
