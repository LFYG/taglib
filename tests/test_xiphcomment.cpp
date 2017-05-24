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

#include <xiphcomment.h>
#include <tpropertymap.h>
#include <vorbisfile.h>
#include <boost/test/unit_test.hpp>
#include "utils.h"
#include "loghelpers.h"

using namespace TagLib;

BOOST_AUTO_TEST_SUITE(TestXiphComment)

BOOST_AUTO_TEST_CASE(testYear)
{
  Ogg::XiphComment cmt;
  BOOST_CHECK_EQUAL(cmt.year(), 0);
  cmt.addField("YEAR", "2009");
  BOOST_CHECK_EQUAL(cmt.year(), 2009);
  cmt.addField("DATE", "2008");
  BOOST_CHECK_EQUAL(cmt.year(), 2008);
}

BOOST_AUTO_TEST_CASE(testSetYear)
{
  Ogg::XiphComment cmt;
  cmt.addField("YEAR", "2009");
  cmt.addField("DATE", "2008");
  cmt.setYear(1995);
  BOOST_CHECK(cmt.fieldListMap()["YEAR"].isEmpty());
  BOOST_CHECK_EQUAL(cmt.fieldListMap()["DATE"].front(), "1995");
}

BOOST_AUTO_TEST_CASE(testTrack)
{
  Ogg::XiphComment cmt;
  BOOST_CHECK_EQUAL(cmt.track(), 0);
  cmt.addField("TRACKNUM", "7");
  BOOST_CHECK_EQUAL(cmt.track(), 7);
  cmt.addField("TRACKNUMBER", "8");
  BOOST_CHECK_EQUAL(cmt.track(), 8);
}

BOOST_AUTO_TEST_CASE(testSetTrack)
{
  Ogg::XiphComment cmt;
  cmt.addField("TRACKNUM", "7");
  cmt.addField("TRACKNUMBER", "8");
  cmt.setTrack(3);
  BOOST_CHECK(cmt.fieldListMap()["TRACKNUM"].isEmpty());
  BOOST_CHECK_EQUAL(cmt.fieldListMap()["TRACKNUMBER"].front(), "3");
}

BOOST_AUTO_TEST_CASE(testInvalidKeys1)
{
  PropertyMap map;
  map[""] = String("invalid key: empty string");
  map["A=B"] = String("invalid key: contains '='");
  map["A~B"] = String("invalid key: contains '~'");
  map["A\x7F" "B"] = String("invalid key: contains '\x7F'");
  map[L"A\x3456" "B"] = String("invalid key: Unicode");
  
  Ogg::XiphComment cmt;
  const PropertyMap unsuccessful = cmt.setProperties(map);
  BOOST_CHECK_EQUAL(unsuccessful.size(), 5);
  BOOST_CHECK(cmt.properties().isEmpty());
}

BOOST_AUTO_TEST_CASE(testInvalidKeys2)
{
  Ogg::XiphComment cmt;
  cmt.addField("", "invalid key: empty string");
  cmt.addField("A=B", "invalid key: contains '='");
  cmt.addField("A~B", "invalid key: contains '~'");
  cmt.addField("A\x7F" "B", "invalid key: contains '\x7F'");
  cmt.addField(L"A\x3456" "B", "invalid key: Unicode");
  BOOST_CHECK_EQUAL(cmt.fieldCount(), 0);
}

BOOST_AUTO_TEST_CASE(testClearComment)
{
  const ScopedFileCopy copy("empty", ".ogg");
  {
    Ogg::Vorbis::File f(copy.fileName());
    f.tag()->addField("COMMENT", "Comment1");
    f.save();
  }
  {
    Ogg::Vorbis::File f(copy.fileName());
    f.tag()->setComment("");
    BOOST_CHECK_EQUAL(f.tag()->comment(), "");
  }
}

BOOST_AUTO_TEST_CASE(testRemoveFields)
{
  Ogg::Vorbis::File f(TEST_FILE_PATH_C("empty.ogg"));
  f.tag()->addField("title", "Title1");
  f.tag()->addField("Title", "Title1", false);
  f.tag()->addField("titlE", "Title2", false);
  f.tag()->addField("TITLE", "Title3", false);
  f.tag()->addField("artist", "Artist1");
  f.tag()->addField("ARTIST", "Artist2", false);
  BOOST_CHECK_EQUAL(f.tag()->title(), "Title1 Title1 Title2 Title3");
  BOOST_CHECK_EQUAL(f.tag()->artist(), "Artist1 Artist2");
  
  f.tag()->removeFields("title", "Title1");
  BOOST_CHECK_EQUAL(f.tag()->title(), "Title2 Title3");
  BOOST_CHECK_EQUAL(f.tag()->artist(), "Artist1 Artist2");
  
  f.tag()->removeFields("Artist");
  BOOST_CHECK_EQUAL(f.tag()->title(), "Title2 Title3");
  BOOST_CHECK(f.tag()->artist().isEmpty());
  
  f.tag()->removeAllFields();
  BOOST_CHECK(f.tag()->title().isEmpty());
  BOOST_CHECK(f.tag()->artist().isEmpty());
  BOOST_CHECK_EQUAL(f.tag()->vendorID(), "Xiph.Org libVorbis I 20050304");
}

BOOST_AUTO_TEST_CASE(testPicture)
{
  const ScopedFileCopy copy("empty", ".ogg");
  {
    Ogg::Vorbis::File f(copy.fileName());
    FLAC::Picture *newpic = new FLAC::Picture();
    newpic->setType(FLAC::Picture::BackCover);
    newpic->setWidth(5);
    newpic->setHeight(6);
    newpic->setColorDepth(16);
    newpic->setNumColors(7);
    newpic->setMimeType("image/jpeg");
    newpic->setDescription("new image");
    newpic->setData("JPEG data");
    f.tag()->addPicture(newpic);
    f.save();
  }
  {
    Ogg::Vorbis::File f(copy.fileName());
    const List<FLAC::Picture *> lst = f.tag()->pictureList();
    BOOST_CHECK_EQUAL(lst.size(), 1);
    BOOST_CHECK_EQUAL(lst[0]->width(), 5);
    BOOST_CHECK_EQUAL(lst[0]->height(), 6);
    BOOST_CHECK_EQUAL(lst[0]->colorDepth(), 16);
    BOOST_CHECK_EQUAL(lst[0]->numColors(), 7);
    BOOST_CHECK_EQUAL(lst[0]->mimeType(), "image/jpeg");
    BOOST_CHECK_EQUAL(lst[0]->description(), "new image");
    BOOST_CHECK_EQUAL(lst[0]->data(), "JPEG data");
  }
}

BOOST_AUTO_TEST_CASE(testLowercaseFields)
{
  const ScopedFileCopy copy("lowercase-fields", ".ogg");
  {
    Ogg::Vorbis::File f(copy.fileName());
    List<FLAC::Picture *> lst = f.tag()->pictureList();
    BOOST_CHECK_EQUAL(f.tag()->title(), "TEST TITLE");
    BOOST_CHECK_EQUAL(f.tag()->artist(), "TEST ARTIST");
    BOOST_CHECK_EQUAL(lst.size(), 1);
    f.save();
  }
  {
    Ogg::Vorbis::File f(copy.fileName());
    BOOST_CHECK(f.find("METADATA_BLOCK_PICTURE") > 0);
  }
}

BOOST_AUTO_TEST_SUITE_END()
