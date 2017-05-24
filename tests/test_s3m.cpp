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

#include <s3mfile.h>
#include <boost/test/unit_test.hpp>
#include "utils.h"
#include "loghelpers.h"

using namespace TagLib;

namespace
{
  const String titleBefore("test song name");
  const String titleAfter("changed title");

  const String commentBefore(
    "This is an instrument name.\n"
    "Module file formats\n"
    "abuse instrument names\n"
    "as multiline comments.\n"
    " ");

  const String newComment(
    "This is an instrument name!\n"
    "Module file formats\n"
    "abuse instrument names\n"
    "as multiline comments.\n"
    "-----------------------------------\n"
    "This line will be dropped and the previous is truncated.");

  const String commentAfter(
    "This is an instrument name!\n"
    "Module file formats\n"
    "abuse instrument names\n"
    "as multiline comments.\n"
    "---------------------------");

  void testRead(FileName fileName, const String &title, const String &comment)
  {
    S3M::File file(fileName);

    BOOST_CHECK(file.isValid());

    S3M::Properties *p = file.audioProperties();
    Mod::Tag *t = file.tag();

    BOOST_CHECK(p);
    BOOST_CHECK(t);

    BOOST_CHECK_EQUAL(p->length(), 0);
    BOOST_CHECK_EQUAL(p->bitrate(), 0);
    BOOST_CHECK_EQUAL(p->sampleRate(), 0);
    BOOST_CHECK_EQUAL(p->channels(), 16);
    BOOST_CHECK_EQUAL(p->lengthInPatterns(), 0);
    BOOST_CHECK(!p->stereo());
    BOOST_CHECK_EQUAL(p->sampleCount(), 5);
    BOOST_CHECK_EQUAL(p->patternCount(), 1);
    BOOST_CHECK_EQUAL(p->flags(), 0);
    BOOST_CHECK_EQUAL(p->trackerVersion(), 4896);
    BOOST_CHECK_EQUAL(p->fileFormatVersion(), 2);
    BOOST_CHECK_EQUAL(p->globalVolume(), 64);
    BOOST_CHECK_EQUAL(p->masterVolume(), 48);
    BOOST_CHECK_EQUAL(p->tempo(), 125);
    BOOST_CHECK_EQUAL(p->bpmSpeed(), 6);
    BOOST_CHECK_EQUAL(t->title(), title);
    BOOST_CHECK(t->artist().isEmpty());
    BOOST_CHECK(t->album().isEmpty());
    BOOST_CHECK_EQUAL(t->comment(), comment);
    BOOST_CHECK(t->genre().isEmpty());
    BOOST_CHECK_EQUAL(t->year(), 0);
    BOOST_CHECK_EQUAL(t->track(), 0);
    BOOST_CHECK_EQUAL(t->trackerName(), "ScreamTracker III");
  }
}

BOOST_AUTO_TEST_SUITE(TestS3M)

BOOST_AUTO_TEST_CASE(testReadTags)
{
  testRead(TEST_FILE_PATH_C("test.s3m"), titleBefore, commentBefore);
}

BOOST_AUTO_TEST_CASE(testWriteTags)
{
  const ScopedFileCopy copy("test", ".s3m");
  {
    S3M::File file(copy.fileName());
    BOOST_CHECK(file.tag());
    file.tag()->setTitle(titleAfter);
    file.tag()->setComment(newComment);
    file.tag()->setTrackerName("won't be saved");
    BOOST_CHECK(file.save());
  }
  testRead(copy.fileName(), titleAfter, commentAfter);
  BOOST_CHECK(fileEqual(copy.fileName(), TEST_FILE_PATH_C("changed.s3m")));
}

BOOST_AUTO_TEST_SUITE_END()
