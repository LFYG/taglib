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

#include <itfile.h>
#include <boost/test/unit_test.hpp>
#include "utils.h"
#include "loghelpers.h"

using namespace TagLib;

namespace
{
  const String titleBefore("test song name");
  const String titleAfter("changed title");

  const String commentBefore(
    "This is a sample name.\n"
    "In module file formats\n"
    "sample names are abused\n"
    "as multiline comments.\n"
    " ");

  const String newComment(
    "This is a sample name!\n"
    "In module file formats\n"
    "sample names are abused\n"
    "as multiline comments.\n"
    "-----------------------------------\n"
    "The previous line is truncated but starting with this line\n"
    "the comment is not limeted in the line length but to 8000\n"
    "additional characters (bytes).\n"
    "\n"
    "This is because it is saved in the 'message' proportion of\n"
    "IT files.");

  const String commentAfter(
    "This is a sample name!\n"
    "In module file formats\n"
    "sample names are abused\n"
    "as multiline comments.\n"
    "-------------------------\n"
    "The previous line is truncated but starting with this line\n"
    "the comment is not limeted in the line length but to 8000\n"
    "additional characters (bytes).\n"
    "\n"
    "This is because it is saved in the 'message' proportion of\n"
    "IT files.");

  void testRead(FileName fileName, const String &title, const String &comment)
  {
    IT::File file(fileName);

    BOOST_CHECK(file.isValid());

    IT::Properties *p = file.audioProperties();
    Mod::Tag *t = file.tag();

    BOOST_CHECK(p);
    BOOST_CHECK(t);

    BOOST_CHECK_EQUAL(p->length(), 0);
    BOOST_CHECK_EQUAL(p->bitrate(), 0);
    BOOST_CHECK_EQUAL(p->sampleRate(), 0);
    BOOST_CHECK_EQUAL(p->channels(), 64);
    BOOST_CHECK_EQUAL(p->lengthInPatterns(), 0);
    BOOST_CHECK(p->stereo());
    BOOST_CHECK_EQUAL(p->instrumentCount(), 0);
    BOOST_CHECK_EQUAL(p->sampleCount(), 5);
    BOOST_CHECK_EQUAL(p->patternCount(), 1);
    BOOST_CHECK_EQUAL(p->version(), 535);
    BOOST_CHECK_EQUAL(p->compatibleVersion(), 532);
    BOOST_CHECK_EQUAL(p->flags(), 9);
    BOOST_CHECK_EQUAL(p->globalVolume(), 128);
    BOOST_CHECK_EQUAL(p->mixVolume(), 48);
    BOOST_CHECK_EQUAL(p->tempo(), 125);
    BOOST_CHECK_EQUAL(p->bpmSpeed(), 6);
    BOOST_CHECK_EQUAL(p->panningSeparation(), 128);
    BOOST_CHECK_EQUAL(p->pitchWheelDepth(), 0);
    BOOST_CHECK_EQUAL(t->title(), title);
    BOOST_CHECK(t->artist().isEmpty());
    BOOST_CHECK(t->album().isEmpty());
    BOOST_CHECK_EQUAL(t->comment(), comment);
    BOOST_CHECK(t->genre().isEmpty());
    BOOST_CHECK_EQUAL(t->year(), 0);
    BOOST_CHECK_EQUAL(t->track(), 0);
    BOOST_CHECK_EQUAL(t->trackerName(), "Impulse Tracker");
  }
}

BOOST_AUTO_TEST_SUITE(TestIT)

BOOST_AUTO_TEST_CASE(testReadTags)
{
  testRead(TEST_FILE_PATH_C("test.it"), titleBefore, commentBefore);
}

BOOST_AUTO_TEST_CASE(testWriteTags)
{
  const ScopedFileCopy copy("test", ".it");
  {
    IT::File file(copy.fileName());
    BOOST_CHECK(file.tag() != 0);
    file.tag()->setTitle(titleAfter);
    file.tag()->setComment(newComment);
    file.tag()->setTrackerName("won't be saved");
    BOOST_CHECK(file.save());
  }
  testRead(copy.fileName(), titleAfter, commentAfter);
}

BOOST_AUTO_TEST_SUITE_END()
