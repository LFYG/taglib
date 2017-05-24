/***************************************************************************
    copyright           : (C) 2007 by Lukas Lalinsky
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

#include <id3v2tag.h>
#include <id3v2frame.h>
#include <tpropertymap.h>
#include <tzlib.h>
#include <attachedpictureframe.h>
#include <chapterframe.h>
#include <eventtimingcodesframe.h>
#include <generalencapsulatedobjectframe.h>
#include <ownershipframe.h>
#include <popularimeterframe.h>
#include <relativevolumeframe.h>
#include <synchronizedlyricsframe.h>
#include <tableofcontentsframe.h>
#include <textidentificationframe.h>
#include <uniquefileidentifierframe.h>
#include <unknownframe.h>
#include <unsynchronizedlyricsframe.h>
#include <urllinkframe.h>
#include <mpegfile.h>
#include <boost/test/unit_test.hpp>
#include "utils.h"
#include "loghelpers.h"

using namespace TagLib;

namespace
{
  class PublicFrame : public ID3v2::Frame
  {
    public:
      PublicFrame() : ID3v2::Frame(ByteVector("XXXX\0\0\0\0\0\0", 10)) {}
      String readStringField(const ByteVector &data, String::Type encoding,
                             int *positon = 0)
        { return ID3v2::Frame::readStringField(data, encoding, positon); }
      virtual String toString() const { return String(); }
      virtual void parseFields(const ByteVector &) {}
      virtual ByteVector renderFields() const { return ByteVector(); }
  };
}

BOOST_AUTO_TEST_SUITE(TestID3v2)

BOOST_AUTO_TEST_CASE(testUnsynchDecode)
{
  MPEG::File f(TEST_FILE_PATH_C("unsynch.id3"), false);
  BOOST_CHECK(f.tag());
  BOOST_CHECK_EQUAL(f.tag()->title(), "My babe just cares for me");
}

BOOST_AUTO_TEST_CASE(testDowngradeUTF8ForID3v23_1)
{
  const ScopedFileCopy copy("xing", ".mp3");
  
  ID3v2::TextIdentificationFrame *f
    = new ID3v2::TextIdentificationFrame(ByteVector("TPE1"), String::UTF8);
  StringList sl;
  sl.append("Foo");
  f->setText(sl);
  
  MPEG::File file(copy.fileName());
  file.ID3v2Tag(true)->addFrame(f);
  file.save(MPEG::File::ID3v2, true, 3);
  BOOST_CHECK_EQUAL(file.hasID3v2Tag(), true);
  
  ByteVector data = f->render();
  BOOST_CHECK_EQUAL(data.size(), 4+4+2+1+6+2);
  
  ID3v2::TextIdentificationFrame f2(data);
  BOOST_CHECK_EQUAL(f2.fieldList(), sl);
  BOOST_CHECK_EQUAL(f2.textEncoding(), String::UTF16);
}

BOOST_AUTO_TEST_CASE(testDowngradeUTF8ForID3v23_2)
{
  const ScopedFileCopy copy("xing", ".mp3");

  ID3v2::UnsynchronizedLyricsFrame *f
    = new ID3v2::UnsynchronizedLyricsFrame(String::UTF8);
  f->setText("Foo");
  
  MPEG::File file(copy.fileName());
  file.ID3v2Tag(true)->addFrame(f);
  file.save(MPEG::File::ID3v2, true, 3);
  BOOST_CHECK(file.hasID3v2Tag());
  
  ByteVector data = f->render();
  BOOST_CHECK_EQUAL(data.size(), 4+4+2+1+3+2+2+6+2);
  
  ID3v2::UnsynchronizedLyricsFrame f2(data);
  BOOST_CHECK_EQUAL(f2.text(), "Foo");
  BOOST_CHECK_EQUAL(f2.textEncoding(), String::UTF16);
}

BOOST_AUTO_TEST_CASE(testUTF16BEDelimiter)
{
  ID3v2::TextIdentificationFrame f(ByteVector("TPE1"), String::UTF16BE);
  StringList sl;
  sl.append("Foo");
  sl.append("Bar");
  f.setText(sl);
  BOOST_CHECK_EQUAL(f.render().size(), 4+4+2+1+6+2+6);
}

BOOST_AUTO_TEST_CASE(testUTF16Delimiter)
{
  ID3v2::TextIdentificationFrame f(ByteVector("TPE1"), String::UTF16);
  StringList sl;
  sl.append("Foo");
  sl.append("Bar");
  f.setText(sl);
  BOOST_CHECK_EQUAL(f.render().size(), 4+4+2+1+8+2+8);
}

BOOST_AUTO_TEST_CASE(testReadStringField)
{
  PublicFrame f;
  ByteVector data("abc\0", 4);
  String str = f.readStringField(data, String::Latin1);
  BOOST_CHECK_EQUAL(str, "abc");
}

BOOST_AUTO_TEST_CASE(testBrokenFrame)
{
  MPEG::File f(TEST_FILE_PATH_C("broken-tenc.id3"), false);
  BOOST_CHECK(f.tag());
  BOOST_CHECK(!f.ID3v2Tag()->frameListMap().contains("TENC"));
}

BOOST_AUTO_TEST_CASE(testParseAPIC)
{
  // http://bugs.kde.org/show_bug.cgi?id=151078
  const ID3v2::AttachedPictureFrame f(ByteVector("APIC"
                                                 "\x00\x00\x00\x07"
                                                 "\x00\x00"
                                                 "\x00"
                                                 "m\x00"
                                                 "\x01"
                                                 "d\x00"
                                                 "\x00", 17));
  BOOST_CHECK_EQUAL(f.mimeType(), "m");
  BOOST_CHECK_EQUAL(f.type(), ID3v2::AttachedPictureFrame::FileIcon);
  BOOST_CHECK_EQUAL(f.description(), "d");
}

BOOST_AUTO_TEST_CASE(testParseAPIC_UTF16_BOM)
{
  const ID3v2::AttachedPictureFrame f(ByteVector(
    "\x41\x50\x49\x43\x00\x02\x0c\x59\x00\x00\x01\x69\x6d\x61\x67\x65"
    "\x2f\x6a\x70\x65\x67\x00\x00\xfe\xff\x00\x63\x00\x6f\x00\x76\x00"
    "\x65\x00\x72\x00\x2e\x00\x6a\x00\x70\x00\x67\x00\x00\xff\xd8\xff",
    16 * 3));
  BOOST_CHECK_EQUAL(f.mimeType(), "image/jpeg");
  BOOST_CHECK_EQUAL(f.type(), ID3v2::AttachedPictureFrame::Other);
  BOOST_CHECK_EQUAL(f.description(), "cover.jpg");
  BOOST_CHECK_EQUAL(f.picture(), "\xff\xd8\xff");
}

BOOST_AUTO_TEST_CASE(testParseAPICv22)
{
  ID3v2::FrameFactory *factory = ID3v2::FrameFactory::instance();
  const ByteVector data = ByteVector("PIC"
                                     "\x00\x00\x08"
                                     "\x00"
                                     "JPG"
                                     "\x01"
                                     "d\x00"
                                     "\x00", 14);
  ID3v2::AttachedPictureFrame *frame =
      dynamic_cast<TagLib::ID3v2::AttachedPictureFrame*>(factory->createFrame(data, 2U));
  
  BOOST_CHECK(frame);
  BOOST_CHECK_EQUAL(frame->mimeType(), "image/jpeg");
  BOOST_CHECK_EQUAL(frame->type(), ID3v2::AttachedPictureFrame::FileIcon);
  BOOST_CHECK_EQUAL(frame->description(), "d");
  
  delete frame;
}

BOOST_AUTO_TEST_CASE(testDontRender22)
{
  ID3v2::FrameFactory *factory = ID3v2::FrameFactory::instance();
  const ByteVector data = ByteVector("FOO"
                                     "\x00\x00\x08"
                                     "\x00"
                                     "JPG"
                                     "\x01"
                                     "d\x00"
                                     "\x00", 14);
  ID3v2::UnknownFrame *frame =
      dynamic_cast<TagLib::ID3v2::UnknownFrame*>(factory->createFrame(data, 2U));
  
  BOOST_CHECK(frame);
  
  ID3v2::Tag tag;
  tag.addFrame(frame);
  BOOST_CHECK_EQUAL(tag.render().size(), 1034);
}

BOOST_AUTO_TEST_CASE(testParseGEOB)
{
  // http://bugs.kde.org/show_bug.cgi?id=151078
  const ID3v2::GeneralEncapsulatedObjectFrame f(ByteVector("GEOB"
                                                           "\x00\x00\x00\x08"
                                                           "\x00\x00"
                                                           "\x00"
                                                           "m\x00"
                                                           "f\x00"
                                                           "d\x00"
                                                           "\x00", 18));
  BOOST_CHECK_EQUAL(f.mimeType(), "m");
  BOOST_CHECK_EQUAL(f.fileName(), "f");
  BOOST_CHECK_EQUAL(f.description(), "d");
}

BOOST_AUTO_TEST_CASE(testParsePOPM)
{
  const ID3v2::PopularimeterFrame f(ByteVector("POPM"
                                               "\x00\x00\x00\x17"
                                               "\x00\x00"
                                               "email@example.com\x00"
                                               "\x02"
                                               "\x00\x00\x00\x03", 33));
  BOOST_CHECK_EQUAL(f.email(), "email@example.com");
  BOOST_CHECK_EQUAL(f.rating(), 2);
  BOOST_CHECK_EQUAL(f.counter(), 3);
}

BOOST_AUTO_TEST_CASE(testParsePOPMWithoutCounter)
{
  const ID3v2::PopularimeterFrame f(ByteVector("POPM"
                                               "\x00\x00\x00\x13"
                                               "\x00\x00"
                                               "email@example.com\x00"
                                               "\x02", 29));
  BOOST_CHECK_EQUAL(f.email(), "email@example.com");
  BOOST_CHECK_EQUAL(f.rating(), 2);
  BOOST_CHECK_EQUAL(f.counter(), 0);
}

BOOST_AUTO_TEST_CASE(testRenderPOPM)
{
  ID3v2::PopularimeterFrame f;
  f.setEmail("email@example.com");
  f.setRating(2);
  f.setCounter(3);
  BOOST_CHECK_EQUAL(
    f.render(),
    ByteVector("POPM"
               "\x00\x00\x00\x17"
               "\x00\x00"
               "email@example.com\x00"
               "\x02"
               "\x00\x00\x00\x03", 33));
}

BOOST_AUTO_TEST_CASE(testPOPMtoString)
{
  ID3v2::PopularimeterFrame f;
  f.setEmail("email@example.com");
  f.setRating(2);
  f.setCounter(3);
  BOOST_CHECK_EQUAL(f.toString(), "email@example.com rating=2 counter=3");
}

BOOST_AUTO_TEST_CASE(testPOPMFromFile)
{
  const ScopedFileCopy copy("xing", ".mp3");
  {
    ID3v2::PopularimeterFrame *f = new ID3v2::PopularimeterFrame();
    f->setEmail("email@example.com");
    f->setRating(200);
    f->setCounter(3);

    MPEG::File foo(copy.fileName());
    foo.ID3v2Tag()->addFrame(f);
    foo.save();
  }
  {
    MPEG::File bar(copy.fileName());
    const ID3v2::PopularimeterFrame *f 
      = dynamic_cast<ID3v2::PopularimeterFrame *>(bar.ID3v2Tag()->frameList("POPM").front());
    BOOST_CHECK_EQUAL(f->email(), "email@example.com");
    BOOST_CHECK_EQUAL(f->rating(), 200);
  }
}

BOOST_AUTO_TEST_CASE(testParseRelativeVolumeFrame)
{
  // http://bugs.kde.org/show_bug.cgi?id=150481
  const ID3v2::RelativeVolumeFrame f(ByteVector("RVA2"              // Frame ID
                                                "\x00\x00\x00\x0B"  // Frame size
                                                "\x00\x00"          // Frame flags
                                                "ident\x00"         // Identification
                                                "\x02"              // Type of channel
                                                "\x00\x0F"          // Volume adjustment
                                                "\x08"              // Bits representing peak
                                                "\x45", 21));       // Peak volume
  BOOST_CHECK_EQUAL(f.identification(), "ident");
  BOOST_CHECK_EQUAL(f.volumeAdjustment(ID3v2::RelativeVolumeFrame::FrontRight), 15.0f / 512.0f);
  BOOST_CHECK_EQUAL(f.peakVolume(ID3v2::RelativeVolumeFrame::FrontRight).bitsRepresentingPeak, 8);
  BOOST_CHECK_EQUAL(f.peakVolume(ID3v2::RelativeVolumeFrame::FrontRight).peakVolume, "\x45");
}

BOOST_AUTO_TEST_CASE(testParseUniqueFileIdentifierFrame)
{
  const ID3v2::UniqueFileIdentifierFrame f(ByteVector("UFID"                 // Frame ID
                                                      "\x00\x00\x00\x09"     // Frame size
                                                      "\x00\x00"             // Frame flags
                                                      "owner\x00"            // Owner identifier
                                                      "\x00\x01\x02", 19));  // Identifier
  BOOST_CHECK_EQUAL(f.owner(), "owner");
  BOOST_CHECK_EQUAL(f.identifier(), ByteVector("\x00\x01\x02", 3));
}

BOOST_AUTO_TEST_CASE(testParseEmptyUniqueFileIdentifierFrame)
{
  const ID3v2::UniqueFileIdentifierFrame f(ByteVector("UFID"                 // Frame ID
                                                      "\x00\x00\x00\x01"     // Frame size
                                                      "\x00\x00"             // Frame flags
                                                      "\x00"                 // Owner identifier
                                                      "", 11));              // Identifier
  BOOST_CHECK(f.owner().isEmpty());
  BOOST_CHECK(f.identifier().isEmpty());
}

BOOST_AUTO_TEST_CASE(testParseUrlLinkFrame)
{
  const ID3v2::UrlLinkFrame f(ByteVector("WOAF"                      // Frame ID
                                         "\x00\x00\x00\x12"          // Frame size
                                         "\x00\x00"                  // Frame flags
                                         "http://example.com", 28)); // URL
  BOOST_CHECK_EQUAL(f.url(), "http://example.com");
}

BOOST_AUTO_TEST_CASE(testRenderUrlLinkFrame)
{
  ID3v2::UrlLinkFrame f("WOAF");
  f.setUrl("http://example.com");
  BOOST_CHECK_EQUAL(
    f.render(),
    ByteVector("WOAF"                      // Frame ID
               "\x00\x00\x00\x12"          // Frame size
               "\x00\x00"                  // Frame flags
               "http://example.com", 28)); // URL
}

BOOST_AUTO_TEST_CASE(testParseUserUrlLinkFrame)
{
  const ID3v2::UserUrlLinkFrame f(ByteVector("WXXX"                      // Frame ID
                                             "\x00\x00\x00\x17"          // Frame size
                                             "\x00\x00"                  // Frame flags
                                             "\x00"                      // Text encoding
                                             "foo\x00"                   // Description
                                             "http://example.com", 33)); // URL
  BOOST_CHECK_EQUAL(f.description(), "foo");
  BOOST_CHECK_EQUAL(f.url(), "http://example.com");
}

BOOST_AUTO_TEST_CASE(testRenderUserUrlLinkFrame)
{
  ID3v2::UserUrlLinkFrame f;
  f.setDescription("foo");
  f.setUrl("http://example.com");
  BOOST_CHECK_EQUAL(
    f.render(),
    ByteVector("WXXX"                      // Frame ID
               "\x00\x00\x00\x17"          // Frame size
               "\x00\x00"                  // Frame flags
               "\x00"                      // Text encoding
               "foo\x00"                   // Description
               "http://example.com", 33)); // URL
}

BOOST_AUTO_TEST_CASE(testParseOwnershipFrame)
{
  const ID3v2::OwnershipFrame f(ByteVector("OWNE"             // Frame ID
                                           "\x00\x00\x00\x19" // Frame size
                                           "\x00\x00"         // Frame flags
                                           "\x00"             // Text encoding
                                           "GBP1.99\x00"      // Price paid
                                           "20120905"         // Date of purchase
                                           "Beatport", 35));  // Seller
  BOOST_CHECK_EQUAL(f.pricePaid(), "GBP1.99");
  BOOST_CHECK_EQUAL(f.datePurchased(), "20120905");
  BOOST_CHECK_EQUAL(f.seller(), "Beatport");
}

BOOST_AUTO_TEST_CASE(testRenderOwnershipFrame)
{
  ID3v2::OwnershipFrame f;
  f.setPricePaid("GBP1.99");
  f.setDatePurchased("20120905");
  f.setSeller("Beatport");
  BOOST_CHECK_EQUAL(
    f.render(),
    ByteVector("OWNE"             // Frame ID
              "\x00\x00\x00\x19"  // Frame size
              "\x00\x00"          // Frame flags
              "\x00"              // Text encoding
              "GBP1.99\x00"       // Price paid
              "20120905"          // Date of purchase
              "Beatport", 35));   // Seller
}

BOOST_AUTO_TEST_CASE(testParseSynchronizedLyricsFrame)
{
  const ID3v2::SynchronizedLyricsFrame f(ByteVector("SYLT"                    // Frame ID
                                                    "\x00\x00\x00\x21"        // Frame size
                                                    "\x00\x00"                // Frame flags
                                                    "\x00"                    // Text encoding
                                                    "eng"                     // Language
                                                    "\x02"                    // Time stamp format
                                                    "\x01"                    // Content type
                                                    "foo\x00"                 // Content descriptor
                                                    "Example\x00"             // 1st text
                                                    "\x00\x00\x04\xd2"        // 1st time stamp
                                                    "Lyrics\x00"              // 2nd text
                                                    "\x00\x00\x11\xd7", 43)); // 2nd time stamp
  BOOST_CHECK_EQUAL(f.textEncoding(), String::Latin1);
  BOOST_CHECK_EQUAL(f.language(), "eng");
  BOOST_CHECK_EQUAL(f.timestampFormat(), ID3v2::SynchronizedLyricsFrame::AbsoluteMilliseconds);
  BOOST_CHECK_EQUAL(f.type(), ID3v2::SynchronizedLyricsFrame::Lyrics);
  BOOST_CHECK_EQUAL(f.description(), "foo");
  const ID3v2::SynchronizedLyricsFrame::SynchedTextList stl = f.synchedText();
  BOOST_CHECK_EQUAL(stl.size(), 2);
  BOOST_CHECK_EQUAL(stl[0].text, "Example");
  BOOST_CHECK_EQUAL(stl[0].time, 1234);
  BOOST_CHECK_EQUAL(stl[1].text, "Lyrics");
  BOOST_CHECK_EQUAL(stl[1].time, 4567);
}

BOOST_AUTO_TEST_CASE(testParseSynchronizedLyricsFrameWithEmptyDescritpion)
{
  const ID3v2::SynchronizedLyricsFrame f(ByteVector("SYLT"                    // Frame ID
                                                    "\x00\x00\x00\x21"        // Frame size
                                                    "\x00\x00"                // Frame flags
                                                    "\x00"                    // Text encoding
                                                    "eng"                     // Language
                                                    "\x02"                    // Time stamp format
                                                    "\x01"                    // Content type
                                                    "\x00"                    // Content descriptor
                                                    "Example\x00"             // 1st text
                                                    "\x00\x00\x04\xd2"        // 1st time stamp
                                                    "Lyrics\x00"              // 2nd text
                                                    "\x00\x00\x11\xd7", 40)); // 2nd time stamp
  BOOST_CHECK_EQUAL(f.textEncoding(), String::Latin1);
  BOOST_CHECK_EQUAL(f.language(), "eng");
  BOOST_CHECK_EQUAL(f.timestampFormat(), ID3v2::SynchronizedLyricsFrame::AbsoluteMilliseconds);
  BOOST_CHECK_EQUAL(f.type(), ID3v2::SynchronizedLyricsFrame::Lyrics);
  BOOST_CHECK(f.description().isEmpty());
  const ID3v2::SynchronizedLyricsFrame::SynchedTextList stl = f.synchedText();
  BOOST_CHECK_EQUAL(stl.size(), 2);
  BOOST_CHECK_EQUAL(stl[0].text, "Example");
  BOOST_CHECK_EQUAL(stl[0].time, 1234);
  BOOST_CHECK_EQUAL(stl[1].text, "Lyrics");
  BOOST_CHECK_EQUAL(stl[1].time, 4567);
}

BOOST_AUTO_TEST_CASE(testRenderSynchronizedLyricsFrame)
{
  ID3v2::SynchronizedLyricsFrame f;
  f.setTextEncoding(String::Latin1);
  f.setLanguage("eng");
  f.setTimestampFormat(ID3v2::SynchronizedLyricsFrame::AbsoluteMilliseconds);
  f.setType(ID3v2::SynchronizedLyricsFrame::Lyrics);
  f.setDescription("foo");
  ID3v2::SynchronizedLyricsFrame::SynchedTextList stl;
  stl.append(ID3v2::SynchronizedLyricsFrame::SynchedText(1234, "Example"));
  stl.append(ID3v2::SynchronizedLyricsFrame::SynchedText(4567, "Lyrics"));
  f.setSynchedText(stl);
  BOOST_CHECK_EQUAL(
    f.render(),
    ByteVector("SYLT"                      // Frame ID
               "\x00\x00\x00\x21"          // Frame size
               "\x00\x00"                  // Frame flags
               "\x00"                      // Text encoding
               "eng"                       // Language
               "\x02"                      // Time stamp format
               "\x01"                      // Content type
               "foo\x00"                   // Content descriptor
               "Example\x00"               // 1st text
               "\x00\x00\x04\xd2"          // 1st time stamp
               "Lyrics\x00"                // 2nd text
               "\x00\x00\x11\xd7", 43));   // 2nd time stamp
}

BOOST_AUTO_TEST_CASE(testParseEventTimingCodesFrame)
{
  const ID3v2::EventTimingCodesFrame f(ByteVector("ETCO"                      // Frame ID
                                                  "\x00\x00\x00\x0b"          // Frame size
                                                  "\x00\x00"                  // Frame flags
                                                  "\x02"                      // Time stamp format
                                                  "\x02"                      // 1st event
                                                  "\x00\x00\xf3\x5c"          // 1st time stamp
                                                  "\xfe"                      // 2nd event
                                                  "\x00\x36\xee\x80", 21));   // 2nd time stamp
  BOOST_CHECK_EQUAL(f.timestampFormat(), ID3v2::EventTimingCodesFrame::AbsoluteMilliseconds);
  ID3v2::EventTimingCodesFrame::SynchedEventList sel = f.synchedEvents();
  BOOST_CHECK_EQUAL(sel.size(), 2);
  BOOST_CHECK_EQUAL(sel[0].type, ID3v2::EventTimingCodesFrame::IntroStart);
  BOOST_CHECK_EQUAL(sel[0].time, 62300);
  BOOST_CHECK_EQUAL(sel[1].type, ID3v2::EventTimingCodesFrame::AudioFileEnds);
  BOOST_CHECK_EQUAL(sel[1].time, 3600000);
}

BOOST_AUTO_TEST_CASE(testRenderEventTimingCodesFrame)
{
  ID3v2::EventTimingCodesFrame f;
  f.setTimestampFormat(ID3v2::EventTimingCodesFrame::AbsoluteMilliseconds);
  ID3v2::EventTimingCodesFrame::SynchedEventList sel;
  sel.append(ID3v2::EventTimingCodesFrame::SynchedEvent(62300, ID3v2::EventTimingCodesFrame::IntroStart));
  sel.append(ID3v2::EventTimingCodesFrame::SynchedEvent(3600000, ID3v2::EventTimingCodesFrame::AudioFileEnds));
  f.setSynchedEvents(sel);
  BOOST_CHECK_EQUAL(
    f.render(),
    ByteVector("ETCO"                      // Frame ID
               "\x00\x00\x00\x0b"          // Frame size
               "\x00\x00"                  // Frame flags
               "\x02"                      // Time stamp format
               "\x02"                      // 1st event
               "\x00\x00\xf3\x5c"          // 1st time stamp
               "\xfe"                      // 2nd event
               "\x00\x36\xee\x80", 21));   // 2nd time stamp
}

BOOST_AUTO_TEST_CASE(testParseChapterFrame)
{
  const ByteVector chapterData("CHAP"                     // Frame ID
                               "\x00\x00\x00\x20"         // Frame size
                               "\x00\x00"                 // Frame flags
                               "\x43\x00"                 // Element ID ("C")
                               "\x00\x00\x00\x03"         // Start time
                               "\x00\x00\x00\x05"         // End time
                               "\x00\x00\x00\x02"         // Start offset
                               "\x00\x00\x00\x03", 28);   // End offset
  const ByteVector embeddedFrameData("TIT2"               // Embedded frame ID
                                     "\x00\x00\x00\x04"   // Embedded frame size
                                     "\x00\x00"           // Embedded frame flags
                                     "\x00"               // TIT2 frame text encoding
                                     "CH1", 14);          // Chapter title
  
  ID3v2::Header header;
  ID3v2::ChapterFrame f1(&header, chapterData);
  BOOST_CHECK_EQUAL(f1.elementID(), ByteVector("C"));
  BOOST_CHECK_EQUAL(f1.startTime(), 3);
  BOOST_CHECK_EQUAL(f1.endTime(), 5);
  BOOST_CHECK_EQUAL(f1.startOffset(), 2);
  BOOST_CHECK_EQUAL(f1.endOffset(), 3);
  BOOST_CHECK(f1.embeddedFrameList().isEmpty());
  
  ID3v2::ChapterFrame f2(&header, chapterData + embeddedFrameData);
  BOOST_CHECK_EQUAL(f2.elementID(), ByteVector("C"));
  BOOST_CHECK_EQUAL(f2.startTime(), 3);
  BOOST_CHECK_EQUAL(f2.endTime(), 5);
  BOOST_CHECK_EQUAL(f2.startOffset(), 2);
  BOOST_CHECK_EQUAL(f2.endOffset(), 3);
  BOOST_CHECK_EQUAL(f2.embeddedFrameList().size(), 1);
  BOOST_CHECK_EQUAL(f2.embeddedFrameList("TIT2").size(), 1);
  BOOST_CHECK_EQUAL(f2.embeddedFrameList("TIT2")[0]->toString(), "CH1");
}

BOOST_AUTO_TEST_CASE(testRenderChapterFrame)
{
  const ID3v2::Header header;
  ID3v2::ChapterFrame f1(&header, "CHAP");
  f1.setElementID(ByteVector("\x43\x00", 2));
  f1.setStartTime(3);
  f1.setEndTime(5);
  f1.setStartOffset(2);
  f1.setEndOffset(3);
  ID3v2::TextIdentificationFrame *eF = new ID3v2::TextIdentificationFrame("TIT2");
  eF->setText("CH1");
  f1.addEmbeddedFrame(eF);
  
  const ByteVector expected("CHAP"                     // Frame ID
                            "\x00\x00\x00\x20"         // Frame size
                            "\x00\x00"                 // Frame flags
                            "\x43\x00"                 // Element ID
                            "\x00\x00\x00\x03"         // Start time
                            "\x00\x00\x00\x05"         // End time
                            "\x00\x00\x00\x02"         // Start offset
                            "\x00\x00\x00\x03"         // End offset
                            "TIT2"                     // Embedded frame ID
                            "\x00\x00\x00\x04"         // Embedded frame size
                            "\x00\x00"                 // Embedded frame flags
                            "\x00"                     // TIT2 frame text encoding
                            "CH1", 42);                // Chapter title
  
  BOOST_CHECK_EQUAL(f1.render(), expected);
  
  f1.setElementID("C");
  
  BOOST_CHECK_EQUAL(f1.render(), expected);
  
  ID3v2::FrameList frames;
  eF = new ID3v2::TextIdentificationFrame("TIT2");
  eF->setText("CH1");
  frames.append(eF);
  
  ID3v2::ChapterFrame f2(ByteVector("\x43\x00", 2), 3, 5, 2, 3, frames);
  BOOST_CHECK_EQUAL(f2.render(), expected);
  
  frames.clear();
  eF = new ID3v2::TextIdentificationFrame("TIT2");
  eF->setText("CH1");
  frames.append(eF);
  
  ID3v2::ChapterFrame f3(ByteVector("C\x00", 2), 3, 5, 2, 3, frames);
  BOOST_CHECK_EQUAL(f3.render(), expected);
  
  frames.clear();
  eF = new ID3v2::TextIdentificationFrame("TIT2");
  eF->setText("CH1");
  frames.append(eF);
  
  ID3v2::ChapterFrame f4("C", 3, 5, 2, 3, frames);
  BOOST_CHECK_EQUAL(f4.render(), expected);
  
  BOOST_CHECK(!f4.toString().isEmpty());
  
  ID3v2::ChapterFrame f5("C", 3, 5, 2, 3);
  eF = new ID3v2::TextIdentificationFrame("TIT2");
  eF->setText("CH1");
  f5.addEmbeddedFrame(eF);
  BOOST_CHECK_EQUAL(f5.render(), expected);
}

BOOST_AUTO_TEST_CASE(testParseTableOfContentsFrame)
{
  const ID3v2::Header header;
  const ID3v2::TableOfContentsFrame f(
    &header,
    ByteVector("CTOC"                     // Frame ID
               "\x00\x00\x00\x16"         // Frame size
               "\x00\x00"                 // Frame flags
               "\x54\x00"                 // Element ID ("T")
               "\x01"                     // CTOC flags
               "\x02"                     // Entry count
               "\x43\x00"                 // First entry ("C")
               "\x44\x00"                 // Second entry ("D")
               "TIT2"                     // Embedded frame ID
               "\x00\x00\x00\x04"         // Embedded frame size
               "\x00\x00"                 // Embedded frame flags
               "\x00"                     // TIT2 frame text encoding
               "TC1", 32));               // Table of contents title
  BOOST_CHECK_EQUAL(f.elementID(), "T");
  BOOST_CHECK(!f.isTopLevel());
  BOOST_CHECK(f.isOrdered());
  BOOST_CHECK_EQUAL(f.entryCount(), 2);
  BOOST_CHECK_EQUAL(f.childElements()[0], "C");
  BOOST_CHECK_EQUAL(f.childElements()[1], "D");
  BOOST_CHECK_EQUAL(f.embeddedFrameList().size(), 1);
  BOOST_CHECK_EQUAL(f.embeddedFrameList("TIT2").size(), 1);
  BOOST_CHECK_EQUAL(f.embeddedFrameList("TIT2")[0]->toString(), "TC1");
}

BOOST_AUTO_TEST_CASE(testParseTableOfContentsFrameWithManyChildren)
{
  MPEG::File f(TEST_FILE_PATH_C("toc_many_children.mp3"));
  BOOST_CHECK(f.isValid());
}

BOOST_AUTO_TEST_CASE(testRenderTableOfContentsFrame)
{
  const ID3v2::Header header;
  ID3v2::TableOfContentsFrame f(&header, "CTOC");
  f.setElementID(ByteVector("\x54\x00", 2));
  f.setIsTopLevel(false);
  f.setIsOrdered(true);
  f.addChildElement(ByteVector("\x43\x00", 2));
  f.addChildElement(ByteVector("\x44\x00", 2));
  ID3v2::TextIdentificationFrame *eF = new ID3v2::TextIdentificationFrame("TIT2");
  eF->setText("TC1");
  f.addEmbeddedFrame(eF);
  BOOST_CHECK_EQUAL(
    f.render(),
    ByteVector("CTOC"                     // Frame ID
               "\x00\x00\x00\x16"         // Frame size
               "\x00\x00"                 // Frame flags
               "\x54\x00"                 // Element ID
               "\x01"                     // CTOC flags
               "\x02"                     // Entry count
               "\x43\x00"                 // First entry
               "\x44\x00"                 // Second entry
               "TIT2"                     // Embedded frame ID
               "\x00\x00\x00\x04"         // Embedded frame size
               "\x00\x00"                 // Embedded frame flags
               "\x00"                     // TIT2 frame text encoding
               "TC1", 32));               // Table of contents title
}

BOOST_AUTO_TEST_CASE(testItunes24FrameSize)
{
  MPEG::File f(TEST_FILE_PATH_C("005411.id3"), false);
  BOOST_CHECK(f.tag());
  const ID3v2::FrameListMap m = f.ID3v2Tag()->frameListMap();
  BOOST_CHECK(m.contains("TIT2"));
  BOOST_CHECK_EQUAL(m["TIT2"].front()->toString(), "Sunshine Superman");
}

BOOST_AUTO_TEST_CASE(testSaveUTF16Comment)
{
  const String::Type defaultEncoding = ID3v2::FrameFactory::instance()->defaultTextEncoding();
  const ScopedFileCopy copy("xing", ".mp3");
  ID3v2::FrameFactory::instance()->setDefaultTextEncoding(String::UTF16);
  {
    MPEG::File foo(copy.fileName());
    foo.strip();
    foo.tag()->setComment("Test comment!");
    foo.save();
  }
  {
    MPEG::File bar(copy.fileName());
    BOOST_CHECK_EQUAL(bar.tag()->comment(), "Test comment!");
    ID3v2::FrameFactory::instance()->setDefaultTextEncoding(defaultEncoding);
  }
}

BOOST_AUTO_TEST_CASE(testUpdateGenre23_1)
{
  // "Refinement" is the same as the ID3v1 genre - duplicate
  ID3v2::FrameFactory *factory = ID3v2::FrameFactory::instance();
  const ByteVector data("TCON"                  // Frame ID
                        "\x00\x00\x00\x10"      // Frame size
                        "\x00\x00"              // Frame flags
                        "\x00"                  // Encoding
                        "(22)Death Metal", 26); // Text
  ID3v2::TextIdentificationFrame *frame =
      dynamic_cast<TagLib::ID3v2::TextIdentificationFrame*>(factory->createFrame(data, 3U));
  BOOST_CHECK_EQUAL(frame->fieldList().size(), 1);
  BOOST_CHECK_EQUAL(frame->fieldList()[0], "Death Metal");
  
  ID3v2::Tag tag;
  tag.addFrame(frame);
  BOOST_CHECK_EQUAL(tag.genre(), "Death Metal");
}

BOOST_AUTO_TEST_CASE(testUpdateGenre23_2)
{
  // "Refinement" is different from the ID3v1 genre
  ID3v2::FrameFactory *factory = ID3v2::FrameFactory::instance();
  const ByteVector data("TCON"                  // Frame ID
                        "\x00\x00\x00\x13"      // Frame size
                        "\x00\x00"              // Frame flags
                        "\x00"                  // Encoding
                        "(4)Eurodisco", 23);    // Text
  ID3v2::TextIdentificationFrame *frame =
      dynamic_cast<TagLib::ID3v2::TextIdentificationFrame*>(factory->createFrame(data, 3U));
  BOOST_CHECK_EQUAL(frame->fieldList().size(), 2);
  BOOST_CHECK_EQUAL(frame->fieldList()[0], "4");
  BOOST_CHECK_EQUAL(frame->fieldList()[1], "Eurodisco");
  
  ID3v2::Tag tag;
  tag.addFrame(frame);
  BOOST_CHECK_EQUAL(tag.genre(), "Disco Eurodisco");
}

BOOST_AUTO_TEST_CASE(testUpdateGenre24)
{
  ID3v2::FrameFactory *factory = ID3v2::FrameFactory::instance();
  const ByteVector data("TCON"                  // Frame ID
                        "\x00\x00\x00\x0D"      // Frame size
                        "\x00\x00"              // Frame flags
                        "\0"                    // Encoding
                        "14\0Eurodisco", 23);   // Text
  ID3v2::TextIdentificationFrame *frame =
      dynamic_cast<TagLib::ID3v2::TextIdentificationFrame*>(factory->createFrame(data, 4U));
  BOOST_CHECK_EQUAL(frame->fieldList().size(), 2);
  BOOST_CHECK_EQUAL(frame->fieldList()[0], "14");
  BOOST_CHECK_EQUAL(frame->fieldList()[1], "Eurodisco");
  
  ID3v2::Tag tag;
  tag.addFrame(frame);
  BOOST_CHECK_EQUAL(tag.genre(), "R&B Eurodisco");
}

BOOST_AUTO_TEST_CASE(testUpdateDate22)
{
  MPEG::File f(TEST_FILE_PATH_C("id3v22-tda.mp3"), false);
  BOOST_CHECK(f.tag());
  BOOST_CHECK_EQUAL(f.tag()->year(), 2010);
}

// TODO: TYE+TDA should be upgraded to TDRC together
//BOOST_AUTO_TEST_CASE(testUpdateFullDate22)
//{
//  MPEG::File f(TEST_FILE_PATH_C("id3v22-tda.mp3"), false);
//  BOOST_CHECK(f.tag());
//  BOOST_CHECK_EQUAL(f.ID3v2Tag()->frameListMap()["TDRC"].front()->toString(), "2010-04-03");
//}

BOOST_AUTO_TEST_CASE(testDowngradeTo23)
{
  const ScopedFileCopy copy("xing", ".mp3");
  
  ID3v2::TextIdentificationFrame *tf;
  {
    MPEG::File foo(copy.fileName());
    tf = new ID3v2::TextIdentificationFrame("TDOR", String::Latin1);
    tf->setText("2011-03-16");
    foo.ID3v2Tag()->addFrame(tf);
    tf = new ID3v2::TextIdentificationFrame("TDRC", String::Latin1);
    tf->setText("2012-04-17T12:01");
    foo.ID3v2Tag()->addFrame(tf);
    tf = new ID3v2::TextIdentificationFrame("TMCL", String::Latin1);
    tf->setText(StringList().append("Guitar").append("Artist 1").append("Drums").append("Artist 2"));
    foo.ID3v2Tag()->addFrame(tf);
    tf = new ID3v2::TextIdentificationFrame("TIPL", String::Latin1);
    tf->setText(StringList().append("Producer").append("Artist 3").append("Mastering").append("Artist 4"));
    foo.ID3v2Tag()->addFrame(tf);
    foo.ID3v2Tag()->addFrame(new ID3v2::TextIdentificationFrame("TDRL", String::Latin1));
    foo.ID3v2Tag()->addFrame(new ID3v2::TextIdentificationFrame("TDTG", String::Latin1));
    foo.ID3v2Tag()->addFrame(new ID3v2::TextIdentificationFrame("TMOO", String::Latin1));
    foo.ID3v2Tag()->addFrame(new ID3v2::TextIdentificationFrame("TPRO", String::Latin1));
    foo.ID3v2Tag()->addFrame(new ID3v2::TextIdentificationFrame("TSOA", String::Latin1));
    foo.ID3v2Tag()->addFrame(new ID3v2::TextIdentificationFrame("TSOT", String::Latin1));
    foo.ID3v2Tag()->addFrame(new ID3v2::TextIdentificationFrame("TSST", String::Latin1));
    foo.ID3v2Tag()->addFrame(new ID3v2::TextIdentificationFrame("TSOP", String::Latin1));
    foo.save(MPEG::File::AllTags, true, 3);
  }
  {
    MPEG::File bar(copy.fileName());
    tf = dynamic_cast<ID3v2::TextIdentificationFrame *>(bar.ID3v2Tag()->frameList("TDOR").front());
    BOOST_CHECK(tf);
    BOOST_CHECK_EQUAL(tf->fieldList().size(), 1);
    BOOST_CHECK_EQUAL(tf->fieldList().front(), "2011");
    tf = dynamic_cast<ID3v2::TextIdentificationFrame *>(bar.ID3v2Tag()->frameList("TDRC").front());
    BOOST_CHECK(tf);
    BOOST_CHECK_EQUAL(tf->fieldList().size(), 1);
    BOOST_CHECK_EQUAL(tf->fieldList().front(), "2012-04-17T12:01");
    tf = dynamic_cast<ID3v2::TextIdentificationFrame *>(bar.ID3v2Tag()->frameList("TIPL").front());
    BOOST_CHECK(tf);
    BOOST_CHECK_EQUAL(tf->fieldList().size(), 8);
    BOOST_CHECK_EQUAL(tf->fieldList()[0], "Guitar");
    BOOST_CHECK_EQUAL(tf->fieldList()[1], "Artist 1");
    BOOST_CHECK_EQUAL(tf->fieldList()[2], "Drums");
    BOOST_CHECK_EQUAL(tf->fieldList()[3], "Artist 2");
    BOOST_CHECK_EQUAL(tf->fieldList()[4], "Producer");
    BOOST_CHECK_EQUAL(tf->fieldList()[5], "Artist 3");
    BOOST_CHECK_EQUAL(tf->fieldList()[6], "Mastering");
    BOOST_CHECK_EQUAL(tf->fieldList()[7], "Artist 4");
    BOOST_CHECK(!bar.ID3v2Tag()->frameListMap().contains("TDRL"));
    BOOST_CHECK(!bar.ID3v2Tag()->frameListMap().contains("TDTG"));
    BOOST_CHECK(!bar.ID3v2Tag()->frameListMap().contains("TMOO"));
    BOOST_CHECK(!bar.ID3v2Tag()->frameListMap().contains("TPRO"));
  #ifdef NO_ITUNES_HACKS
    BOOST_CHECK(!bar.ID3v2Tag()->frameListMap().contains("TSOA"));
    BOOST_CHECK(!bar.ID3v2Tag()->frameListMap().contains("TSOT"));
    BOOST_CHECK(!bar.ID3v2Tag()->frameListMap().contains("TSOP"));
  #endif
    BOOST_CHECK(!bar.ID3v2Tag()->frameListMap().contains("TSST"));
  }
}

BOOST_AUTO_TEST_CASE(testCompressedFrameWithBrokenLength)
{
  MPEG::File f(TEST_FILE_PATH_C("compressed_id3_frame.mp3"), false);
  BOOST_CHECK(f.ID3v2Tag()->frameListMap().contains("APIC"));
  
  if(zlib::isAvailable()) {
    ID3v2::AttachedPictureFrame *frame
      = dynamic_cast<TagLib::ID3v2::AttachedPictureFrame*>(f.ID3v2Tag()->frameListMap()["APIC"].front());
    BOOST_CHECK(frame);
    BOOST_CHECK_EQUAL(frame->mimeType(), "image/bmp");
    BOOST_CHECK_EQUAL(frame->type(), ID3v2::AttachedPictureFrame::Other);
    BOOST_CHECK(frame->description().isEmpty());
    BOOST_CHECK_EQUAL(frame->picture().size(), 86414);
  }
  else {
    // Skip the test if ZLIB is not installed.
    // The message "Compressed frames are currently not supported." will be displayed.
  
    ID3v2::UnknownFrame *frame
      = dynamic_cast<TagLib::ID3v2::UnknownFrame*>(f.ID3v2Tag()->frameListMap()["APIC"].front());
    BOOST_CHECK(frame);
  }
}

BOOST_AUTO_TEST_CASE(testW000)
{
  MPEG::File f(TEST_FILE_PATH_C("w000.mp3"), false);
  BOOST_CHECK(f.ID3v2Tag()->frameListMap().contains("W000"));
  const ID3v2::UrlLinkFrame *frame =
    dynamic_cast<TagLib::ID3v2::UrlLinkFrame*>(f.ID3v2Tag()->frameListMap()["W000"].front());
  BOOST_CHECK(frame);
  BOOST_CHECK_EQUAL(frame->url(), "lukas.lalinsky@example.com____");
}

BOOST_AUTO_TEST_CASE(testPropertyInterface)
{
  const ScopedFileCopy copy("rare_frames", ".mp3");
  MPEG::File f(copy.fileName());
  PropertyMap dict = f.ID3v2Tag(false)->properties();
  BOOST_CHECK_EQUAL(dict.size(), 6);
  
  BOOST_CHECK(dict.contains("USERTEXTDESCRIPTION1"));
  BOOST_CHECK(dict.contains("QuodLibet::USERTEXTDESCRIPTION2"));
  BOOST_CHECK_EQUAL(dict["USERTEXTDESCRIPTION1"].size(), 2);
  BOOST_CHECK_EQUAL(dict["QuodLibet::USERTEXTDESCRIPTION2"].size(), 2);
  BOOST_CHECK_EQUAL(dict["USERTEXTDESCRIPTION1"][0], "userTextData1");
  BOOST_CHECK_EQUAL(dict["USERTEXTDESCRIPTION1"][1], "userTextData2");
  BOOST_CHECK_EQUAL(dict["QuodLibet::USERTEXTDESCRIPTION2"][0], "userTextData1");
  BOOST_CHECK_EQUAL(dict["QuodLibet::USERTEXTDESCRIPTION2"][1], "userTextData2");
  
  BOOST_CHECK_EQUAL(dict["GENRE"].front(), "Pop");
  
  BOOST_CHECK_EQUAL(dict["URL:USERURL"].front(), "http://a.user.url");
  
  BOOST_CHECK_EQUAL(dict["URL"].front(), "http://a.user.url/with/empty/description");
  BOOST_CHECK_EQUAL(dict["COMMENT"].front(), "A COMMENT");
  
  BOOST_CHECK_EQUAL(dict.unsupportedData().size(), 1);
  BOOST_CHECK_EQUAL(dict.unsupportedData().front(), "UFID/supermihi@web.de");
}

BOOST_AUTO_TEST_CASE(testPropertyInterface2)
{
  ID3v2::Tag tag;
  ID3v2::UnsynchronizedLyricsFrame *frame1 = new ID3v2::UnsynchronizedLyricsFrame();
  frame1->setDescription("test");
  frame1->setText("la-la-la test");
  tag.addFrame(frame1);
  
  ID3v2::UnsynchronizedLyricsFrame *frame2 = new ID3v2::UnsynchronizedLyricsFrame();
  frame2->setDescription("");
  frame2->setText("la-la-la nodescription");
  tag.addFrame(frame2);
  
  ID3v2::AttachedPictureFrame *frame3 = new ID3v2::AttachedPictureFrame();
  frame3->setDescription("test picture");
  tag.addFrame(frame3);
  
  ID3v2::TextIdentificationFrame *frame4 = new ID3v2::TextIdentificationFrame("TIPL");
  frame4->setText("single value is invalid for TIPL");
  tag.addFrame(frame4);
  
  ID3v2::TextIdentificationFrame *frame5 = new ID3v2::TextIdentificationFrame("TMCL");
  StringList tmclData;
  tmclData.append("VIOLIN");
  tmclData.append("a violinist");
  tmclData.append("PIANO");
  tmclData.append("a pianist");
  frame5->setText(tmclData);
  tag.addFrame(frame5);
  
  ID3v2::UniqueFileIdentifierFrame *frame6 = new ID3v2::UniqueFileIdentifierFrame("http://musicbrainz.org", "152454b9-19ba-49f3-9fc9-8fc26545cf41");
  tag.addFrame(frame6);
  
  ID3v2::UniqueFileIdentifierFrame *frame7 = new ID3v2::UniqueFileIdentifierFrame("http://example.com", "123");
  tag.addFrame(frame7);
  
  ID3v2::UserTextIdentificationFrame *frame8 = new ID3v2::UserTextIdentificationFrame();
  frame8->setDescription("MusicBrainz Album Id");
  frame8->setText("95c454a5-d7e0-4d8f-9900-db04aca98ab3");
  tag.addFrame(frame8);
  
  PropertyMap properties = tag.properties();
  
  BOOST_CHECK_EQUAL(properties.unsupportedData().size(), 3);
  BOOST_CHECK(properties.unsupportedData().contains("TIPL"));
  BOOST_CHECK(properties.unsupportedData().contains("APIC"));
  BOOST_CHECK(properties.unsupportedData().contains("UFID/http://example.com"));
  
  BOOST_CHECK(properties.contains("PERFORMER:VIOLIN"));
  BOOST_CHECK(properties.contains("PERFORMER:PIANO"));
  BOOST_CHECK_EQUAL(properties["PERFORMER:VIOLIN"].front(), "a violinist");
  BOOST_CHECK_EQUAL(properties["PERFORMER:PIANO"].front(), "a pianist");
  
  BOOST_CHECK(properties.contains("LYRICS"));
  BOOST_CHECK(properties.contains("LYRICS:TEST"));
  
  BOOST_CHECK(properties.contains("MUSICBRAINZ_TRACKID"));
  BOOST_CHECK_EQUAL(properties["MUSICBRAINZ_TRACKID"].front(), "152454b9-19ba-49f3-9fc9-8fc26545cf41");
  
  BOOST_CHECK(properties.contains("MUSICBRAINZ_ALBUMID"));
  BOOST_CHECK_EQUAL(properties["MUSICBRAINZ_ALBUMID"].front(), "95c454a5-d7e0-4d8f-9900-db04aca98ab3");
  
  tag.removeUnsupportedProperties(properties.unsupportedData());
  BOOST_CHECK(tag.frameList("APIC").isEmpty());
  BOOST_CHECK(tag.frameList("TIPL").isEmpty());
  BOOST_CHECK_EQUAL(ID3v2::UniqueFileIdentifierFrame::findByOwner(&tag, "http://example.com"), (ID3v2::UniqueFileIdentifierFrame *)0);
  BOOST_CHECK_EQUAL(ID3v2::UniqueFileIdentifierFrame::findByOwner(&tag, "http://musicbrainz.org"), frame6);
}

BOOST_AUTO_TEST_CASE(testPropertiesMovement)
{
  ID3v2::Tag tag;
  ID3v2::TextIdentificationFrame *frameMvnm = new ID3v2::TextIdentificationFrame("MVNM");
  frameMvnm->setText("Movement Name");
  tag.addFrame(frameMvnm);
  
  ID3v2::TextIdentificationFrame *frameMvin = new ID3v2::TextIdentificationFrame("MVIN");
  frameMvin->setText("2/3");
  tag.addFrame(frameMvin);
  
  PropertyMap properties = tag.properties();
  BOOST_CHECK(properties.contains("MOVEMENTNAME"));
  BOOST_CHECK(properties.contains("MOVEMENTNUMBER"));
  BOOST_CHECK_EQUAL(properties["MOVEMENTNAME"].front(), "Movement Name");
  BOOST_CHECK_EQUAL(properties["MOVEMENTNUMBER"].front(), "2/3");

  const ByteVector frameDataMvnm("MVNM"
                                 "\x00\x00\x00\x0e"
                                 "\x00\x00"
                                 "\x00"
                                 "Movement Name", 24);
  BOOST_CHECK_EQUAL(frameMvnm->render(), frameDataMvnm);
  const ByteVector frameDataMvin("MVIN"
                                 "\x00\x00\x00\x04"
                                 "\x00\x00"
                                 "\x00"
                                 "2/3", 14);
  BOOST_CHECK_EQUAL(frameMvin->render(), frameDataMvin);
  
  ID3v2::FrameFactory *factory = ID3v2::FrameFactory::instance();
  ID3v2::TextIdentificationFrame *parsedFrameMvnm =
      dynamic_cast<ID3v2::TextIdentificationFrame *>(factory->createFrame(frameDataMvnm));
  ID3v2::TextIdentificationFrame *parsedFrameMvin =
      dynamic_cast<ID3v2::TextIdentificationFrame *>(factory->createFrame(frameDataMvin));
  BOOST_CHECK(parsedFrameMvnm);
  BOOST_CHECK(parsedFrameMvin);
  BOOST_CHECK_EQUAL(parsedFrameMvnm->toString(), "Movement Name");
  BOOST_CHECK_EQUAL(parsedFrameMvin->toString(), "2/3");

  tag.addFrame(parsedFrameMvnm);
  tag.addFrame(parsedFrameMvin);
}

BOOST_AUTO_TEST_CASE(testDeleteFrame)
{
  const ScopedFileCopy copy("rare_frames", ".mp3");
  {
    MPEG::File f(copy.fileName());
    ID3v2::Tag *t = f.ID3v2Tag();
    ID3v2::Frame *frame = t->frameList("TCON")[0];
    BOOST_CHECK_EQUAL(t->frameList("TCON").size(), 1);
    t->removeFrame(frame, true);
    f.save(MPEG::File::ID3v2);
  }
  {
    MPEG::File f(copy.fileName());
    BOOST_CHECK(f.ID3v2Tag()->frameList("TCON").isEmpty());
  }
}

BOOST_AUTO_TEST_CASE(testSaveAndStripID3v1ShouldNotAddFrameFromID3v1ToId3v2)
{
  const ScopedFileCopy copy("rare_frames", ".mp3");
  {
    MPEG::File foo(copy.fileName());
    foo.tag()->setArtist("Artist");
    foo.save(MPEG::File::ID3v1 | MPEG::File::ID3v2);
  }
  {
    MPEG::File bar(copy.fileName());
    BOOST_CHECK(bar.hasID3v1Tag());
    BOOST_CHECK(bar.hasID3v2Tag());
    bar.ID3v2Tag()->removeFrames("TPE1");
    // Should strip ID3v1 here and not add old values to ID3v2 again
    bar.save(MPEG::File::ID3v2, true);
  }
  {
    MPEG::File f(copy.fileName());
    BOOST_CHECK(!f.hasID3v1Tag());
    BOOST_CHECK(f.hasID3v2Tag());
    BOOST_CHECK(!f.ID3v2Tag()->frameListMap().contains("TPE1"));
  }
}

BOOST_AUTO_TEST_CASE(testShrinkPadding)
{
  const ScopedFileCopy copy("xing", ".mp3");
  {
    MPEG::File f(copy.fileName());
    f.ID3v2Tag()->setTitle(longText(64 * 1024));
    f.save(MPEG::File::ID3v2, true);
  }
  {
    MPEG::File f(copy.fileName());
    BOOST_CHECK(f.hasID3v2Tag());
    BOOST_CHECK_EQUAL(f.length(), 74789);
    f.ID3v2Tag()->setTitle("ABCDEFGHIJ");
    f.save(MPEG::File::ID3v2, true);
  }
  {
    MPEG::File f(copy.fileName());
    BOOST_CHECK(f.hasID3v2Tag());
    BOOST_CHECK_EQUAL(f.length(), 9263);
  }
}

BOOST_AUTO_TEST_CASE(testEmptyFrame)
{
  const ScopedFileCopy copy("xing", ".mp3");
  {
    MPEG::File f(copy.fileName());
    ID3v2::Tag *tag = f.ID3v2Tag(true);
  
    ID3v2::UrlLinkFrame *frame1 = new ID3v2::UrlLinkFrame(
      ByteVector("WOAF\x00\x00\x00\x01\x00\x00\x00", 11));
    tag->addFrame(frame1);
  
    ID3v2::TextIdentificationFrame *frame2 = new ID3v2::TextIdentificationFrame("TIT2");
    frame2->setText("Title");
    tag->addFrame(frame2);
  
    f.save();
  }
  {
    MPEG::File f(copy.fileName());
    BOOST_CHECK_EQUAL(f.hasID3v2Tag(), true);
  
    ID3v2::Tag *tag = f.ID3v2Tag();
    BOOST_CHECK_EQUAL(tag->title(), "Title");
    BOOST_CHECK_EQUAL(tag->frameListMap()["WOAF"].isEmpty(), true);
  }
}

BOOST_AUTO_TEST_CASE(testDuplicateTags)
{
  const ScopedFileCopy copy("duplicate_id3v2", ".mp3");
  ByteVector audioStream;
  {
    MPEG::File f(copy.fileName());
    f.seek(f.ID3v2Tag()->header()->completeTagSize());
    audioStream = f.readBlock(2089);
  
    // duplicate_id3v2.mp3 has duplicate ID3v2 tags.
    // Sample rate will be 32000 if we can't skip the second tag.
  
    BOOST_CHECK(f.hasID3v2Tag());
    BOOST_CHECK_EQUAL(f.ID3v2Tag()->header()->completeTagSize(), 8049);
  
    BOOST_CHECK_EQUAL(f.audioProperties()->sampleRate(), 44100);
  
    f.ID3v2Tag()->setArtist("Artist A");
    f.save(MPEG::File::ID3v2, true);
  }
  {
    MPEG::File f(copy.fileName());
    BOOST_CHECK(f.hasID3v2Tag());
    BOOST_CHECK_EQUAL(f.length(), 3594);
    BOOST_CHECK_EQUAL(f.ID3v2Tag()->header()->completeTagSize(), 1505);
    BOOST_CHECK_EQUAL(f.ID3v2Tag()->artist(), "Artist A");
    BOOST_CHECK_EQUAL(f.audioProperties()->sampleRate(), 44100);
  
    f.seek(f.ID3v2Tag()->header()->completeTagSize());
    BOOST_CHECK_EQUAL(audioStream, f.readBlock(2089));
  }
}

BOOST_AUTO_TEST_SUITE_END()
