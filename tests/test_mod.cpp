/***************************************************************************
    copyright           : (C) 2011 by Mathias Panzenböck
    email               : grosser.meister.morti@gmx.net
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

#include <modfile.h>
#include <tpropertymap.h>
#include <boost/test/unit_test.hpp>
#include "utils.h"
#include "loghelpers.h"

using namespace TagLib;

namespace
{
  const String titleBefore("title of song");
  const String titleAfter("changed title");

  const String commentBefore(
    "Instrument names\n"
    "are abused as\n"
    "comments in\n"
    "module file formats.\n"
    "-+-+-+-+-+-+-+-+-+-+-+\n"
    "\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n");

  const String newComment(
    "This line will be truncated because it is too long for a mod instrument name.\n"
    "This line is ok.");

  const String commentAfter(
    "This line will be trun\n"
    "This line is ok.\n"
    "\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n");

  void testRead(FileName fileName, const String &title, const String &comment)
  {
    Mod::File file(fileName);
  
    BOOST_CHECK(file.isValid());
  
    Mod::Properties *p = file.audioProperties();
    Mod::Tag *t = file.tag();
  
    BOOST_CHECK(p);
    BOOST_CHECK(t);
  
    BOOST_CHECK_EQUAL(p->length(), 0);
    BOOST_CHECK_EQUAL(p->bitrate(), 0);
    BOOST_CHECK_EQUAL(p->sampleRate(), 0);
    BOOST_CHECK_EQUAL(p->channels(), 8);
    BOOST_CHECK_EQUAL(p->instrumentCount(), 31);
    BOOST_CHECK_EQUAL(p->lengthInPatterns(), 1);
    BOOST_CHECK_EQUAL(t->title(), title);
    BOOST_CHECK(t->artist().isEmpty());
    BOOST_CHECK(t->album().isEmpty());
    BOOST_CHECK_EQUAL(t->comment(), comment);
    BOOST_CHECK(t->genre().isEmpty());
    BOOST_CHECK_EQUAL(t->year(), 0);
    BOOST_CHECK_EQUAL(t->track(), 0);
    BOOST_CHECK_EQUAL(t->trackerName(), "StarTrekker");
  }
}

BOOST_AUTO_TEST_SUITE(TestMod)

BOOST_AUTO_TEST_CASE(testReadTags)
{
  testRead(TEST_FILE_PATH_C("test.mod"), titleBefore, commentBefore);
}

BOOST_AUTO_TEST_CASE(testWriteTags)
{
  const ScopedFileCopy copy("test", ".mod");
  {
    Mod::File file(copy.fileName());
    BOOST_CHECK(file.tag());
    file.tag()->setTitle(titleAfter);
    file.tag()->setComment(newComment);
    BOOST_CHECK(file.save());
  }
  testRead(copy.fileName(), titleAfter, commentAfter);
  BOOST_CHECK(fileEqual(copy.fileName(), TEST_FILE_PATH_C("changed.mod")));
}

BOOST_AUTO_TEST_CASE(testPropertyInterface)
{
  Mod::Tag t;
  PropertyMap properties;
  properties["BLA"] = String("bla");
  properties["ARTIST"] = String("artist1");
  properties["ARTIST"].append("artist2");
  properties["TITLE"] = String("title");
  
  const PropertyMap unsupported = t.setProperties(properties);
  BOOST_CHECK(unsupported.contains("BLA"));
  BOOST_CHECK(unsupported.contains("ARTIST"));
  BOOST_CHECK_EQUAL(unsupported["ARTIST"], properties["ARTIST"]);
  BOOST_CHECK(!unsupported.contains("TITLE"));
}

BOOST_AUTO_TEST_SUITE_END()
