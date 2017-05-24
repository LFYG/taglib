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

#include <xmfile.h>
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

  const String trackerNameBefore("MilkyTracker        ");
  const String trackerNameAfter("TagLib");

  const String commentBefore(
    "Instrument names\n"
    "are abused as\n"
    "comments in\n"
    "module file formats.\n"
    "-+-+-+-+-+-+-+-+-+-+-+\n"
    "\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n"
    "\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n"
    "\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n"
    "\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n"
    "\n\n\n"
    "Sample\n"
    "names\n"
    "are sometimes\n"
    "also abused as\n"
    "comments.");

  const String newCommentShort(
    "Instrument names\n"
    "are abused as\n"
    "comments in\n"
    "module file formats.\n"
    "======================\n"
    "\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n"
    "\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n"
    "\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n"
    "\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n"
    "\n\n\n"
    "Sample names\n"
    "are sometimes\n"
    "also abused as\n"
    "comments.");

  const String newCommentLong(
    "Instrument names\n"
    "are abused as\n"
    "comments in\n"
    "module file formats.\n"
    "======================\n"
    "\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n"
    "\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n"
    "\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n"
    "\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n"
    "\n\n\n"
    "Sample names\n"
    "are sometimes\n"
    "also abused as\n"
    "comments.\n"
    "\n\n\n\n\n\n\n"
    "TEST");

  const String commentAfter(
    "Instrument names\n"
    "are abused as\n"
    "comments in\n"
    "module file formats.\n"
    "======================\n"
    "\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n"
    "\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n"
    "\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n"
    "\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n"
    "\n\n\n"
    "Sample names\n"
    "are sometimes\n"
    "also abused as\n"
    "comments.\n");

  void testRead(FileName fileName, const String &title,
                const String &comment, const String &trackerName)
  {
    XM::File file(fileName);

    BOOST_CHECK(file.isValid());

    XM::Properties *p = file.audioProperties();
    Mod::Tag *t = file.tag();

    BOOST_CHECK(p);
    BOOST_CHECK(t);

    BOOST_CHECK_EQUAL(p->length(), 0);
    BOOST_CHECK_EQUAL(p->bitrate(), 0);
    BOOST_CHECK_EQUAL(p->sampleRate(), 0);
    BOOST_CHECK_EQUAL(p->channels(), 8);
    BOOST_CHECK_EQUAL(p->lengthInPatterns(), 1);
    BOOST_CHECK_EQUAL(p->version(), 260);
    BOOST_CHECK_EQUAL(p->restartPosition(), 0);
    BOOST_CHECK_EQUAL(p->patternCount(), 1);
    BOOST_CHECK_EQUAL(p->instrumentCount(), 128);
    BOOST_CHECK_EQUAL(p->flags(), 1);
    BOOST_CHECK_EQUAL(p->tempo(), 6);
    BOOST_CHECK_EQUAL(p->bpmSpeed(), 125);
    BOOST_CHECK_EQUAL(t->title(), title);
    BOOST_CHECK(t->artist().isEmpty());
    BOOST_CHECK(t->album().isEmpty());
    BOOST_CHECK_EQUAL(t->comment(), comment);
    BOOST_CHECK(t->genre().isEmpty());
    BOOST_CHECK_EQUAL(t->year(), 0);
    BOOST_CHECK_EQUAL(t->track(), 0);
    BOOST_CHECK_EQUAL(t->trackerName(), trackerName);
  }

  void testWriteTags(const String &comment)
  {
    const ScopedFileCopy copy("test", ".xm");
    {
      XM::File file(copy.fileName());
      BOOST_CHECK(file.tag());
      file.tag()->setTitle(titleAfter);
      file.tag()->setComment(comment);
      file.tag()->setTrackerName(trackerNameAfter);
      BOOST_CHECK(file.save());
    }
    testRead(copy.fileName(), titleAfter, commentAfter, trackerNameAfter);
    BOOST_CHECK(fileEqual(copy.fileName(), TEST_FILE_PATH_C("changed.xm")));
  }
}

BOOST_AUTO_TEST_SUITE(TestXM)

BOOST_AUTO_TEST_CASE(testReadTags)
{
  testRead(TEST_FILE_PATH_C("test.xm"), titleBefore, commentBefore, trackerNameBefore);
}

BOOST_AUTO_TEST_CASE(testReadStrippedTags)
{
  XM::File file(TEST_FILE_PATH_C("stripped.xm"));
  BOOST_CHECK(file.isValid());

  XM::Properties *p = file.audioProperties();
  Mod::Tag *t = file.tag();

  BOOST_CHECK(p);
  BOOST_CHECK(t);

  BOOST_CHECK_EQUAL(p->length(), 0);
  BOOST_CHECK_EQUAL(p->bitrate(), 0);
  BOOST_CHECK_EQUAL(p->sampleRate(), 0);
  BOOST_CHECK_EQUAL(p->channels(), 8);
  BOOST_CHECK_EQUAL(p->lengthInPatterns(), 1);
  BOOST_CHECK_EQUAL(p->version(), 0);
  BOOST_CHECK_EQUAL(p->restartPosition(), 0);
  BOOST_CHECK_EQUAL(p->patternCount(), 1);
  BOOST_CHECK_EQUAL(p->instrumentCount(), 0);
  BOOST_CHECK_EQUAL(p->flags(), 1);
  BOOST_CHECK_EQUAL(p->tempo(), 6);
  BOOST_CHECK_EQUAL(p->bpmSpeed(), 125);
  BOOST_CHECK_EQUAL(t->title(), titleBefore);
  BOOST_CHECK(t->artist().isEmpty());
  BOOST_CHECK(t->album().isEmpty());
  BOOST_CHECK(t->comment().isEmpty());
  BOOST_CHECK(t->genre().isEmpty());
  BOOST_CHECK_EQUAL(t->year(), 0);
  BOOST_CHECK_EQUAL(t->track(), 0);
  BOOST_CHECK(t->trackerName().isEmpty());
}

BOOST_AUTO_TEST_CASE(testWriteTagsShort)
{
  testWriteTags(newCommentShort);
}

BOOST_AUTO_TEST_CASE(testWriteTagsLong)
{
  testWriteTags(newCommentLong);
}

BOOST_AUTO_TEST_SUITE_END()
