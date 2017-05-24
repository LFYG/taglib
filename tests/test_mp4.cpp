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

#include <mp4atom.h>
#include <mp4file.h>
#include <tpropertymap.h>
#include <boost/test/unit_test.hpp>
#include "utils.h"
#include "loghelpers.h"

using namespace TagLib;

BOOST_AUTO_TEST_SUITE(TestMP4)

BOOST_AUTO_TEST_CASE(testPropertiesAAC)
{
  MP4::File f(TEST_FILE_PATH_C("has-tags.m4a"));
  BOOST_CHECK(f.audioProperties());
  BOOST_CHECK_EQUAL(f.audioProperties()->length(), 3);
  BOOST_CHECK_EQUAL(f.audioProperties()->lengthInSeconds(), 3);
  BOOST_CHECK_EQUAL(f.audioProperties()->lengthInMilliseconds(), 3708);
  BOOST_CHECK_EQUAL(f.audioProperties()->bitrate(), 3);
  BOOST_CHECK_EQUAL(f.audioProperties()->channels(), 2);
  BOOST_CHECK_EQUAL(f.audioProperties()->sampleRate(), 44100);
  BOOST_CHECK_EQUAL(f.audioProperties()->bitsPerSample(), 16);
  BOOST_CHECK(!f.audioProperties()->isEncrypted());
  BOOST_CHECK_EQUAL(f.audioProperties()->codec(), MP4::Properties::AAC);
}

BOOST_AUTO_TEST_CASE(testPropertiesALAC)
{
  MP4::File f(TEST_FILE_PATH_C("empty_alac.m4a"));
  BOOST_CHECK(f.audioProperties());
  BOOST_CHECK_EQUAL(f.audioProperties()->length(), 3);
  BOOST_CHECK_EQUAL(f.audioProperties()->lengthInSeconds(), 3);
  BOOST_CHECK_EQUAL(f.audioProperties()->lengthInMilliseconds(), 3705);
  BOOST_CHECK_EQUAL(f.audioProperties()->bitrate(), 3);
  BOOST_CHECK_EQUAL(f.audioProperties()->channels(), 2);
  BOOST_CHECK_EQUAL(f.audioProperties()->sampleRate(), 44100);
  BOOST_CHECK_EQUAL(f.audioProperties()->bitsPerSample(), 16);
  BOOST_CHECK(!f.audioProperties()->isEncrypted());
  BOOST_CHECK_EQUAL(f.audioProperties()->codec(), MP4::Properties::ALAC);
}

BOOST_AUTO_TEST_CASE(testPropertiesM4V)
{
  MP4::File f(TEST_FILE_PATH_C("blank_video.m4v"));
  BOOST_CHECK(f.audioProperties());
  BOOST_CHECK_EQUAL(f.audioProperties()->length(), 0);
  BOOST_CHECK_EQUAL(f.audioProperties()->lengthInSeconds(), 0);
  BOOST_CHECK_EQUAL(f.audioProperties()->lengthInMilliseconds(), 975);
  BOOST_CHECK_EQUAL(f.audioProperties()->bitrate(), 96);
  BOOST_CHECK_EQUAL(f.audioProperties()->channels(), 2);
  BOOST_CHECK_EQUAL(f.audioProperties()->sampleRate(), 44100);
  BOOST_CHECK_EQUAL(f.audioProperties()->bitsPerSample(), 16);
  BOOST_CHECK(!f.audioProperties()->isEncrypted());
  BOOST_CHECK_EQUAL(f.audioProperties()->codec(), MP4::Properties::AAC);
}

BOOST_AUTO_TEST_CASE(testFreeForm)
{
  const ScopedFileCopy copy("has-tags", ".m4a");
  {
    MP4::File f(copy.fileName());
    BOOST_CHECK(f.tag()->contains("----:com.apple.iTunes:iTunNORM"));
    f.tag()->setItem("----:org.kde.TagLib:Foo", StringList("Bar"));
    f.save();
  }
  {
    MP4::File f(copy.fileName());
    BOOST_CHECK(f.tag()->contains("----:org.kde.TagLib:Foo"));
    BOOST_CHECK_EQUAL(f.tag()->item("----:org.kde.TagLib:Foo").toStringList().front(), "Bar");
    f.save();
  }
}

BOOST_AUTO_TEST_CASE(testCheckValid)
{
  MP4::File f(TEST_FILE_PATH_C("empty.aiff"));
  BOOST_CHECK(!f.isValid());
}

BOOST_AUTO_TEST_CASE(testHasTag)
{
  {
    MP4::File f(TEST_FILE_PATH_C("has-tags.m4a"));
    BOOST_CHECK(f.isValid());
    BOOST_CHECK(f.hasMP4Tag());
  }
  
  const ScopedFileCopy copy("no-tags", ".m4a");
  {
    MP4::File f(copy.fileName());
    BOOST_CHECK(f.isValid());
    BOOST_CHECK(!f.hasMP4Tag());
    f.tag()->setTitle("TITLE");
    f.save();
  }
  {
    MP4::File f(copy.fileName());
    BOOST_CHECK(f.isValid());
    BOOST_CHECK(f.hasMP4Tag());
  }
}

BOOST_AUTO_TEST_CASE(testIsEmpty)
{
  MP4::Tag t1;
  BOOST_CHECK(t1.isEmpty());
  t1.setArtist("Foo");
  BOOST_CHECK(!t1.isEmpty());
  
  MP4::Tag t2;
  t2.setItem("foo", "bar");
  BOOST_CHECK(!t2.isEmpty());
}

BOOST_AUTO_TEST_CASE(testUpdateStco)
{
  const ScopedFileCopy copy("no-tags", ".3g2");
  ByteVectorList data1;
  {
    MP4::File f(copy.fileName());
    f.tag()->setArtist(ByteVector(3000, 'x'));
  
    MP4::Atoms a(&f);
    MP4::Atom *stco = a.find("moov")->findall("stco", true)[0];
    f.seek(stco->offset + 12);
    ByteVector data = f.readBlock(stco->length - 12);
    unsigned int count = data.mid(0, 4).toUInt();
    int pos = 4;
    while (count--) {
      unsigned int offset = data.mid(pos, 4).toUInt();
      f.seek(offset);
      data1.append(f.readBlock(20));
      pos += 4;
    }
  
    f.save();
  }
  {
    MP4::File f(copy.fileName());

    MP4::Atoms a(&f);
    MP4::Atom *stco = a.find("moov")->findall("stco", true)[0];
    f.seek(stco->offset + 12);
    ByteVector data = f.readBlock(stco->length - 12);
    unsigned int count = data.mid(0, 4).toUInt();
    int pos = 4, i = 0;
    while (count--) {
      unsigned int offset = data.mid(pos, 4).toUInt();
      f.seek(offset);
      BOOST_CHECK_EQUAL(f.readBlock(20), data1[i]);
      pos += 4;
      i++;
    }
  }
}

BOOST_AUTO_TEST_CASE(testSaveExisingWhenIlstIsLast)
{
  const ScopedFileCopy copy("ilst-is-last", ".m4a");
  {
    MP4::File f(copy.fileName());
    BOOST_CHECK_EQUAL(
      f.tag()->item("----:com.apple.iTunes:replaygain_track_minmax").toStringList().front(),
      "82,164");
    BOOST_CHECK_EQUAL(f.tag()->artist(), "Pearl Jam");
    f.tag()->setComment("foo");
    f.save();
  }
  {
    MP4::File f(copy.fileName());
    BOOST_CHECK_EQUAL(
      f.tag()->item("----:com.apple.iTunes:replaygain_track_minmax").toStringList().front(),
      "82,164");
    BOOST_CHECK_EQUAL(f.tag()->artist(), "Pearl Jam");
    BOOST_CHECK_EQUAL(f.tag()->comment(), "foo");
  }
}

BOOST_AUTO_TEST_CASE(test64BitAtom)
{
  const ScopedFileCopy copy("64bit", ".mp4");
  {
    MP4::File f(copy.fileName());
    BOOST_CHECK(f.tag()->itemMap()["cpil"].toBool());
  
    MP4::Atoms atoms(&f);
    MP4::Atom *moov = atoms.atoms[0];
    BOOST_CHECK_EQUAL(moov->length, 77);
  
    f.tag()->setItem("pgap", true);
    f.save();
  }
  {
    MP4::File f(copy.fileName());
    BOOST_CHECK(f.tag()->item("cpil").toBool());
    BOOST_CHECK(f.tag()->item("pgap").toBool());
  
    MP4::Atoms atoms(&f);
    MP4::Atom *moov = atoms.atoms[0];
    // original size + 'pgap' size + padding
    BOOST_CHECK_EQUAL(moov->length, 77 + 25 + 974);
  }
}

BOOST_AUTO_TEST_CASE(testGnre)
{
  MP4::File f(TEST_FILE_PATH_C("gnre.m4a"));
  BOOST_CHECK_EQUAL(f.tag()->genre(), "Ska");
}

BOOST_AUTO_TEST_CASE(testCovrRead1)
{
  MP4::File f(TEST_FILE_PATH_C("has-tags.m4a"));
  BOOST_CHECK(f.tag()->contains("covr"));
  MP4::CoverArtList l = f.tag()->item("covr").toCoverArtList();
  BOOST_CHECK_EQUAL(l.size(), 2);
  BOOST_CHECK_EQUAL(l[0].format(), MP4::CoverArt::PNG);
  BOOST_CHECK_EQUAL(l[0].data().size(), 79);
  BOOST_CHECK_EQUAL(l[1].format(), MP4::CoverArt::JPEG);
  BOOST_CHECK_EQUAL(l[1].data().size(), 287);
}

BOOST_AUTO_TEST_CASE(testCovrRead2)
{
  MP4::File f(TEST_FILE_PATH_C("covr-junk.m4a"));
  BOOST_CHECK(f.tag()->contains("covr"));
  MP4::CoverArtList l = f.tag()->item("covr").toCoverArtList();
  BOOST_CHECK_EQUAL(l.size(), 2);
  BOOST_CHECK_EQUAL(l[0].format(), MP4::CoverArt::PNG);
  BOOST_CHECK_EQUAL(l[0].data().size(), 79);
  BOOST_CHECK_EQUAL(l[1].format(), MP4::CoverArt::JPEG);
  BOOST_CHECK_EQUAL(l[1].data().size(), 287);
}

BOOST_AUTO_TEST_CASE(testCovrWrite)
{
  const ScopedFileCopy copy("has-tags", ".m4a");
  {
    MP4::File f(copy.fileName());
    BOOST_CHECK(f.tag()->contains("covr"));
    MP4::CoverArtList l = f.tag()->item("covr").toCoverArtList();
    l.append(MP4::CoverArt(MP4::CoverArt::PNG, "foo"));
    f.tag()->setItem("covr", l);
    f.save();
  }
  {
    MP4::File f(copy.fileName());
    BOOST_CHECK(f.tag()->contains("covr"));
    MP4::CoverArtList l = f.tag()->item("covr").toCoverArtList();
    BOOST_CHECK_EQUAL(l.size(), 3);
    BOOST_CHECK_EQUAL(l[0].format(), MP4::CoverArt::PNG);
    BOOST_CHECK_EQUAL(l[0].data().size(), 79);
    BOOST_CHECK_EQUAL(l[1].format(), MP4::CoverArt::JPEG);
    BOOST_CHECK_EQUAL(l[1].data().size(), 287);
    BOOST_CHECK_EQUAL(l[2].format(), MP4::CoverArt::PNG);
    BOOST_CHECK_EQUAL(l[2].data().size(), 3);
  }
}

BOOST_AUTO_TEST_CASE(testProperties)
{
  MP4::File f(TEST_FILE_PATH_C("has-tags.m4a"));
  
  PropertyMap tags = f.properties();
  
  BOOST_CHECK_EQUAL(tags["ARTIST"], StringList("Test Artist"));
  
  tags["TRACKNUMBER"] = StringList("2/4");
  tags["DISCNUMBER"] = StringList("3/5");
  tags["BPM"] = StringList("123");
  tags["ARTIST"] = StringList("Foo Bar");
  tags["COMPILATION"] = StringList("1");
  f.setProperties(tags);
  
  tags = f.properties();
  
  BOOST_CHECK(f.tag()->contains("trkn"));
  BOOST_CHECK_EQUAL(f.tag()->item("trkn").toIntPair().first, 2);
  BOOST_CHECK_EQUAL(f.tag()->item("trkn").toIntPair().second, 4);
  BOOST_CHECK_EQUAL(tags["TRACKNUMBER"], StringList("2/4"));
  
  BOOST_CHECK(f.tag()->contains("disk"));
  BOOST_CHECK_EQUAL(f.tag()->item("disk").toIntPair().first, 3);
  BOOST_CHECK_EQUAL(f.tag()->item("disk").toIntPair().second, 5);
  BOOST_CHECK_EQUAL(tags["DISCNUMBER"], StringList("3/5"));
  
  BOOST_CHECK(f.tag()->contains("tmpo"));
  BOOST_CHECK_EQUAL(f.tag()->item("tmpo").toInt(), 123);
  BOOST_CHECK_EQUAL(tags["BPM"], StringList("123"));
  
  BOOST_CHECK(f.tag()->contains("\251ART"));
  BOOST_CHECK_EQUAL(f.tag()->item("\251ART").toStringList(), StringList("Foo Bar"));
  BOOST_CHECK_EQUAL(tags["ARTIST"], StringList("Foo Bar"));
  
  BOOST_CHECK(f.tag()->contains("cpil"));
  BOOST_CHECK(f.tag()->item("cpil").toBool());
  BOOST_CHECK_EQUAL(tags["COMPILATION"], StringList("1"));
  
  tags["COMPILATION"] = StringList("0");
  f.setProperties(tags);
  
  tags = f.properties();
  
  BOOST_CHECK(f.tag()->contains("cpil"));
  BOOST_CHECK(!f.tag()->item("cpil").toBool());
  BOOST_CHECK_EQUAL(tags["COMPILATION"], StringList("0"));
  
  // Empty properties do not result in access violations
  // when converting integers
  tags["TRACKNUMBER"] = StringList();
  tags["DISCNUMBER"] = StringList();
  tags["BPM"] = StringList();
  tags["COMPILATION"] = StringList();
  f.setProperties(tags);
}

BOOST_AUTO_TEST_CASE(testPropertiesMovement)
{
  MP4::File f(TEST_FILE_PATH_C("has-tags.m4a"));
  
  PropertyMap tags = f.properties();
  
  tags["WORK"] = StringList("Foo");
  tags["MOVEMENTNAME"] = StringList("Bar");
  tags["MOVEMENTNUMBER"] = StringList("2");
  tags["MOVEMENTCOUNT"] = StringList("3");
  tags["SHOWWORKMOVEMENT"] = StringList("1");
  f.setProperties(tags);
  
  tags = f.properties();
  
  BOOST_CHECK(f.tag()->contains("\251wrk"));
  BOOST_CHECK_EQUAL(f.tag()->item("\251wrk").toStringList(), StringList("Foo"));
  BOOST_CHECK_EQUAL(tags["WORK"], StringList("Foo"));
  
  BOOST_CHECK(f.tag()->contains("\251mvn"));
  BOOST_CHECK_EQUAL(f.tag()->item("\251mvn").toStringList(), StringList("Bar"));
  BOOST_CHECK_EQUAL(tags["MOVEMENTNAME"], StringList("Bar"));
  
  BOOST_CHECK(f.tag()->contains("\251mvi"));
  BOOST_CHECK_EQUAL(f.tag()->item("\251mvi").toInt(), 2);
  BOOST_CHECK_EQUAL(tags["MOVEMENTNUMBER"], StringList("2"));
  
  BOOST_CHECK(f.tag()->contains("\251mvc"));
  BOOST_CHECK_EQUAL(f.tag()->item("\251mvc").toInt(), 3);
  BOOST_CHECK_EQUAL(tags["MOVEMENTCOUNT"], StringList("3"));
  
  BOOST_CHECK(f.tag()->contains("shwm"));
  BOOST_CHECK(f.tag()->item("shwm").toBool());
  BOOST_CHECK_EQUAL(tags["SHOWWORKMOVEMENT"], StringList("1"));
  
  tags["SHOWWORKMOVEMENT"] = StringList("0");
  f.setProperties(tags);
  
  tags = f.properties();
  
  BOOST_CHECK(f.tag()->contains("shwm"));
  BOOST_CHECK(!f.tag()->item("shwm").toBool());
  BOOST_CHECK_EQUAL(tags["SHOWWORKMOVEMENT"], StringList("0"));
  
  tags["WORK"] = StringList();
  tags["MOVEMENTNAME"] = StringList();
  tags["MOVEMENTNUMBER"] = StringList();
  tags["MOVEMENTCOUNT"] = StringList();
  tags["SHOWWORKMOVEMENT"] = StringList();
  f.setProperties(tags);
}

BOOST_AUTO_TEST_CASE(testFuzzedFile)
{
  MP4::File f(TEST_FILE_PATH_C("infloop.m4a"));
  BOOST_CHECK(f.isValid());
}

BOOST_AUTO_TEST_CASE(testRepeatedSave)
{
  const ScopedFileCopy copy("no-tags", ".m4a");
  
  MP4::File f(copy.fileName());
  f.tag()->setTitle("0123456789");
  f.save();
  f.save();
  BOOST_CHECK_EQUAL(f.find("0123456789"), 2862);
  BOOST_CHECK_EQUAL(f.find("0123456789", 2863), -1);
}

BOOST_AUTO_TEST_CASE(testWithZeroLengthAtom)
{
  MP4::File f(TEST_FILE_PATH_C("zero-length-mdat.m4a"));
  BOOST_CHECK(f.isValid());
  BOOST_CHECK_EQUAL(f.audioProperties()->lengthInMilliseconds(), 1115);
  BOOST_CHECK_EQUAL(f.audioProperties()->sampleRate(), 22050);
}

BOOST_AUTO_TEST_SUITE_END()
