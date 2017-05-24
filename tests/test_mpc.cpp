/***************************************************************************
    copyright           : (C) 2012 by Lukas Lalinsky
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

#include <mpcfile.h>
#include <apetag.h>
#include <id3v1tag.h>
#include <tpropertymap.h>
#include <boost/test/unit_test.hpp>
#include "utils.h"
#include "loghelpers.h"

using namespace TagLib;

BOOST_AUTO_TEST_SUITE(TestMPC)

BOOST_AUTO_TEST_CASE(testPropertiesSV8)
{
  MPC::File f(TEST_FILE_PATH_C("sv8_header.mpc"));
  BOOST_CHECK(f.audioProperties());
  BOOST_CHECK_EQUAL(f.audioProperties()->mpcVersion(), 8);
  BOOST_CHECK_EQUAL(f.audioProperties()->length(), 1);
  BOOST_CHECK_EQUAL(f.audioProperties()->lengthInSeconds(), 1);
  BOOST_CHECK_EQUAL(f.audioProperties()->lengthInMilliseconds(), 1497);
  BOOST_CHECK_EQUAL(f.audioProperties()->bitrate(), 1);
  BOOST_CHECK_EQUAL(f.audioProperties()->channels(), 2);
  BOOST_CHECK_EQUAL(f.audioProperties()->sampleRate(), 44100);
  BOOST_CHECK_EQUAL(f.audioProperties()->sampleFrames(), 66014);
}

BOOST_AUTO_TEST_CASE(testPropertiesSV7)
{
  MPC::File f(TEST_FILE_PATH_C("click.mpc"));
  BOOST_CHECK(f.audioProperties());
  BOOST_CHECK_EQUAL(f.audioProperties()->mpcVersion(), 7);
  BOOST_CHECK_EQUAL(f.audioProperties()->length(), 0);
  BOOST_CHECK_EQUAL(f.audioProperties()->lengthInSeconds(), 0);
  BOOST_CHECK_EQUAL(f.audioProperties()->lengthInMilliseconds(), 40);
  BOOST_CHECK_EQUAL(f.audioProperties()->bitrate(), 318);
  BOOST_CHECK_EQUAL(f.audioProperties()->channels(), 2);
  BOOST_CHECK_EQUAL(f.audioProperties()->sampleRate(), 44100);
  BOOST_CHECK_EQUAL(f.audioProperties()->sampleFrames(), 1760);
  BOOST_CHECK_EQUAL(f.audioProperties()->trackGain(), 14221);
  BOOST_CHECK_EQUAL(f.audioProperties()->trackPeak(), 19848);
  BOOST_CHECK_EQUAL(f.audioProperties()->albumGain(), 14221);
  BOOST_CHECK_EQUAL(f.audioProperties()->albumPeak(), 19848);
}

BOOST_AUTO_TEST_CASE(testPropertiesSV5)
{
  MPC::File f(TEST_FILE_PATH_C("sv5_header.mpc"));
  BOOST_CHECK(f.audioProperties());
  BOOST_CHECK_EQUAL(f.audioProperties()->mpcVersion(), 5);
  BOOST_CHECK_EQUAL(f.audioProperties()->length(), 26);
  BOOST_CHECK_EQUAL(f.audioProperties()->lengthInSeconds(), 26);
  BOOST_CHECK_EQUAL(f.audioProperties()->lengthInMilliseconds(), 26371);
  BOOST_CHECK_EQUAL(f.audioProperties()->bitrate(), 0);
  BOOST_CHECK_EQUAL(f.audioProperties()->channels(), 2);
  BOOST_CHECK_EQUAL(f.audioProperties()->sampleRate(), 44100);
  BOOST_CHECK_EQUAL(f.audioProperties()->sampleFrames(), 1162944);
}

BOOST_AUTO_TEST_CASE(testPropertiesSV4)
{
  MPC::File f(TEST_FILE_PATH_C("sv4_header.mpc"));
  BOOST_CHECK(f.audioProperties());
  BOOST_CHECK_EQUAL(f.audioProperties()->mpcVersion(), 4);
  BOOST_CHECK_EQUAL(f.audioProperties()->length(), 26);
  BOOST_CHECK_EQUAL(f.audioProperties()->lengthInSeconds(), 26);
  BOOST_CHECK_EQUAL(f.audioProperties()->lengthInMilliseconds(), 26371);
  BOOST_CHECK_EQUAL(f.audioProperties()->bitrate(), 0);
  BOOST_CHECK_EQUAL(f.audioProperties()->channels(), 2);
  BOOST_CHECK_EQUAL(f.audioProperties()->sampleRate(), 44100);
  BOOST_CHECK_EQUAL(f.audioProperties()->sampleFrames(), 1162944);
}

BOOST_AUTO_TEST_CASE(testFuzzedFile1)
{
  MPC::File f(TEST_FILE_PATH_C("zerodiv.mpc"));
  BOOST_CHECK(f.isValid());
}

BOOST_AUTO_TEST_CASE(testFuzzedFile2)
{
  MPC::File f(TEST_FILE_PATH_C("infloop.mpc"));
  BOOST_CHECK(f.isValid());
}

BOOST_AUTO_TEST_CASE(testFuzzedFile3)
{
  MPC::File f(TEST_FILE_PATH_C("segfault.mpc"));
  BOOST_CHECK(f.isValid());
}

BOOST_AUTO_TEST_CASE(testFuzzedFile4)
{
  MPC::File f(TEST_FILE_PATH_C("segfault2.mpc"));
  BOOST_CHECK(f.isValid());
}

BOOST_AUTO_TEST_CASE(testStripAndProperties)
{
  const ScopedFileCopy copy("click", ".mpc");
  {
    MPC::File f(copy.fileName());
    f.APETag(true)->setTitle("APE");
    f.ID3v1Tag(true)->setTitle("ID3v1");
    f.save();
  }
  {
    MPC::File f(copy.fileName());
    BOOST_CHECK_EQUAL(f.properties()["TITLE"].front(), "APE");
    f.strip(MPC::File::APE);
    BOOST_CHECK_EQUAL(f.properties()["TITLE"].front(), "ID3v1");
    f.strip(MPC::File::ID3v1);
    BOOST_CHECK(f.properties().isEmpty());
  }
}

BOOST_AUTO_TEST_CASE(testRepeatedSave)
{
  const ScopedFileCopy copy("click", ".mpc");
  {
    MPC::File f(copy.fileName());
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
    MPC::File f(copy.fileName());
    BOOST_CHECK(f.hasAPETag());
    BOOST_CHECK(f.hasID3v1Tag());
  }
}

BOOST_AUTO_TEST_SUITE_END()
