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

#include <vorbisfile.h>
#include <oggpageheader.h>
#include <tpropertymap.h>
#include <boost/test/unit_test.hpp>
#include "utils.h"
#include "loghelpers.h"

using namespace TagLib;

BOOST_AUTO_TEST_SUITE(TestOGG)

BOOST_AUTO_TEST_CASE(testAudioProperties)
{
  Ogg::Vorbis::File f(TEST_FILE_PATH_C("empty.ogg"));
  BOOST_CHECK(f.audioProperties());
  BOOST_CHECK_EQUAL(f.audioProperties()->length(), 3);
  BOOST_CHECK_EQUAL(f.audioProperties()->lengthInSeconds(), 3);
  BOOST_CHECK_EQUAL(f.audioProperties()->lengthInMilliseconds(), 3685);
  BOOST_CHECK_EQUAL(f.audioProperties()->bitrate(), 9);
  BOOST_CHECK_EQUAL(f.audioProperties()->channels(), 2);
  BOOST_CHECK_EQUAL(f.audioProperties()->sampleRate(), 44100);
  BOOST_CHECK_EQUAL(f.audioProperties()->vorbisVersion(), 0);
  BOOST_CHECK_EQUAL(f.audioProperties()->bitrateMaximum(), 0);
  BOOST_CHECK_EQUAL(f.audioProperties()->bitrateNominal(), 112000);
  BOOST_CHECK_EQUAL(f.audioProperties()->bitrateMinimum(), 0);
}

BOOST_AUTO_TEST_CASE(testSimple)
{
  const ScopedFileCopy copy("empty", ".ogg");
  {
    Vorbis::File f(copy.fileName());
    f.tag()->setArtist("The Artist");
    f.save();
  }
  {
    Vorbis::File f(copy.fileName());
    BOOST_CHECK_EQUAL(f.tag()->artist(), "The Artist");
  }
}

BOOST_AUTO_TEST_CASE(testSplitPackets1)
{
  const ScopedFileCopy copy("empty", ".ogg");
  const String text = longText(128 * 1024, true);
  {
    Vorbis::File f(copy.fileName());
    f.tag()->setTitle(text);
    f.save();
  }
  {
    Vorbis::File f(copy.fileName());
    BOOST_CHECK(f.isValid());
    BOOST_CHECK_EQUAL(f.length(), 136383);
    BOOST_CHECK_EQUAL(f.lastPageHeader()->pageSequenceNumber(), 19);
    BOOST_CHECK_EQUAL(f.packet(0).size(), 30);
    BOOST_CHECK_EQUAL(f.packet(1).size(), 131127);
    BOOST_CHECK_EQUAL(f.packet(2).size(), 3832);
    BOOST_CHECK_EQUAL(f.tag()->title(), text);
  
    BOOST_CHECK(f.audioProperties());
    BOOST_CHECK_EQUAL(f.audioProperties()->lengthInMilliseconds(), 3685);
  
    f.tag()->setTitle("ABCDE");
    f.save();
  }
  {
    Vorbis::File f(copy.fileName());
    BOOST_CHECK(f.isValid());
    BOOST_CHECK_EQUAL(f.length(), 4370);
    BOOST_CHECK_EQUAL(f.lastPageHeader()->pageSequenceNumber(), 3);
    BOOST_CHECK_EQUAL(f.packet(0).size(), 30);
    BOOST_CHECK_EQUAL(f.packet(1).size(), 60);
    BOOST_CHECK_EQUAL(f.packet(2).size(), 3832);
    BOOST_CHECK_EQUAL(f.tag()->title(), "ABCDE");
  
    BOOST_CHECK(f.audioProperties());
    BOOST_CHECK_EQUAL(f.audioProperties()->lengthInMilliseconds(), 3685);
  }
}

BOOST_AUTO_TEST_CASE(testSplitPackets2)
{
  const ScopedFileCopy copy("empty", ".ogg");
  const String text = longText(60890, true);
  {
    Vorbis::File f(copy.fileName());
    f.tag()->setTitle(text);
    f.save();
  }
  {
    Vorbis::File f(copy.fileName());
    BOOST_CHECK(f.isValid());
    BOOST_CHECK_EQUAL(f.tag()->title(), text);
  
    f.tag()->setTitle("ABCDE");
    f.save();
  }
  {
    Vorbis::File f(copy.fileName());
    BOOST_CHECK(f.isValid());
    BOOST_CHECK_EQUAL(f.tag()->title(), "ABCDE");
  }
}

BOOST_AUTO_TEST_CASE(testDictInterface1)
{
  const ScopedFileCopy copy("empty", ".ogg");
 
  Vorbis::File f(copy.fileName());
  BOOST_CHECK(f.tag()->properties().isEmpty());
  
  PropertyMap newTags;
  StringList values("value 1");
  values.append("value 2");
  newTags["ARTIST"] = values;
  f.tag()->setProperties(newTags);
  
  const PropertyMap map = f.tag()->properties();
  BOOST_CHECK_EQUAL(map.size(), 1);
  BOOST_CHECK_EQUAL(map["ARTIST"].size(), 2);
  BOOST_CHECK_EQUAL(map["ARTIST"][0], "value 1");
}

BOOST_AUTO_TEST_CASE(testDictInterface2)
{
  const ScopedFileCopy copy("test", ".ogg");
  
  Vorbis::File f(copy.fileName());
  PropertyMap tags = f.tag()->properties();
  
  BOOST_CHECK_EQUAL(tags["UNUSUALTAG"].size(), 2);
  BOOST_CHECK_EQUAL(tags["UNUSUALTAG"][0], "usual value");
  BOOST_CHECK_EQUAL(tags["UNUSUALTAG"][1], "another value");
  BOOST_CHECK_EQUAL(
    tags["UNICODETAG"][0],
    String("\xC3\xB6\xC3\xA4\xC3\xBC\x6F\xCE\xA3\xC3\xB8", String::UTF8));
  
  tags["UNICODETAG"][0] = String(
    "\xCE\xBD\xCE\xB5\xCF\x89\x20\xCE\xBD\xCE\xB1\xCE\xBB\xCF\x85\xCE\xB5", String::UTF8);
  tags.erase("UNUSUALTAG");
  f.tag()->setProperties(tags);
  BOOST_CHECK_EQUAL(
    f.tag()->properties()["UNICODETAG"][0],
    String("\xCE\xBD\xCE\xB5\xCF\x89\x20\xCE\xBD\xCE\xB1\xCE\xBB\xCF\x85\xCE\xB5", String::UTF8));
  BOOST_CHECK(!f.tag()->properties().contains("UNUSUALTAG"));
}

BOOST_AUTO_TEST_CASE(testPageChecksum)
{
  const ScopedFileCopy copy("empty", ".ogg");
  {
    Vorbis::File f(copy.fileName());
    f.tag()->setArtist("The Artist");
    f.save();
  
    f.seek(0x50);
    BOOST_CHECK_EQUAL(f.readBlock(4).toUInt(0, true), 0x3d3bd92dU);
  }
  {
    Vorbis::File f(copy.fileName());
    f.tag()->setArtist("The Artist 2");
    f.save();
  
    f.seek(0x50);
    BOOST_CHECK_EQUAL(f.readBlock(4).toUInt(0, true), 0xd985291cU);
  }
}

BOOST_AUTO_TEST_SUITE_END()
