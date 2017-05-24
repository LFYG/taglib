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

#include <rifffile.h>
#include <boost/test/unit_test.hpp>
#include "utils.h"
#include "loghelpers.h"

using namespace TagLib;

namespace
{
  class PublicRIFF : public RIFF::File
  {
  public:
    PublicRIFF(FileName file) : RIFF::File(file, BigEndian) {};
    unsigned int riffSize() { return RIFF::File::riffSize(); };
    unsigned int chunkCount() { return RIFF::File::chunkCount(); };
    unsigned int chunkOffset(unsigned int i) { return RIFF::File::chunkOffset(i); };
    unsigned int chunkPadding(unsigned int i) { return RIFF::File::chunkPadding(i); };
    unsigned int chunkDataSize(unsigned int i) { return RIFF::File::chunkDataSize(i); };
    ByteVector chunkName(unsigned int i) { return RIFF::File::chunkName(i); };
    ByteVector chunkData(unsigned int i) { return RIFF::File::chunkData(i); };
    void setChunkData(unsigned int i, const ByteVector &data) {
      RIFF::File::setChunkData(i, data);
    }
    void setChunkData(const ByteVector &name, const ByteVector &data) {
      RIFF::File::setChunkData(name, data);
    };
    virtual TagLib::Tag* tag() const { return 0; };
    virtual TagLib::AudioProperties* audioProperties() const { return 0;};
    virtual bool save() { return false; };
    void removeChunk(unsigned int i) { RIFF::File::removeChunk(i); }
    void removeChunk(const ByteVector &name) { RIFF::File::removeChunk(name); }
  };
}

BOOST_AUTO_TEST_SUITE(TestRIFF)

BOOST_AUTO_TEST_CASE(testPadding)
{
  const ScopedFileCopy copy("empty", ".aiff");
  {
    PublicRIFF f(copy.fileName());
    BOOST_CHECK_EQUAL(f.chunkName(2), "TEST");
    BOOST_CHECK_EQUAL(f.chunkOffset(2), 0x1728 + 8);
  
    f.setChunkData("TEST", "foo");
  }
  {
    PublicRIFF f(copy.fileName());
    BOOST_CHECK_EQUAL(f.chunkName(2), "TEST");
    BOOST_CHECK_EQUAL(f.chunkData(2), "foo");
    BOOST_CHECK_EQUAL(f.chunkDataSize(2), 3);
    BOOST_CHECK_EQUAL(f.chunkOffset(2), 0x1728 + 8);
  
    f.setChunkData("SSND", "abcd");
  
    BOOST_CHECK_EQUAL(f.chunkName(1), "SSND");
    BOOST_CHECK_EQUAL(f.chunkData(1), "abcd");
  
    f.seek(f.chunkOffset(1));
    BOOST_CHECK_EQUAL(f.readBlock(4), "abcd");
  
    BOOST_CHECK_EQUAL(f.chunkName(2), "TEST");
    BOOST_CHECK_EQUAL(f.chunkData(2), "foo");
  
    f.seek(f.chunkOffset(2));
    BOOST_CHECK_EQUAL(f.readBlock(3), "foo");
  }
  {
    PublicRIFF f(copy.fileName());
    BOOST_CHECK_EQUAL(f.chunkName(1), "SSND");
    BOOST_CHECK_EQUAL(f.chunkData(1), "abcd");
  
    BOOST_CHECK_EQUAL(f.chunkName(2), "TEST");
    BOOST_CHECK_EQUAL(f.chunkData(2), "foo");
  }
}

BOOST_AUTO_TEST_CASE(testLastChunkAtEvenPosition1)
{
  const ScopedFileCopy copy("noise", ".aif");
  {
    PublicRIFF f(copy.fileName());
    BOOST_CHECK_EQUAL(f.chunkOffset(2), 0xff0 + 8);
    BOOST_CHECK_EQUAL(f.chunkDataSize(2), 311);
    BOOST_CHECK_EQUAL(f.chunkName(2), "SSND");
    BOOST_CHECK_EQUAL(f.chunkPadding(2), 1);
    BOOST_CHECK_EQUAL(f.length(), 4400);
    BOOST_CHECK_EQUAL(f.riffSize(), 4399 - 8);
    f.setChunkData("TEST", "abcd");
    BOOST_CHECK_EQUAL(f.chunkOffset(2), 4088);
    BOOST_CHECK_EQUAL(f.chunkDataSize(2), 311);
    BOOST_CHECK_EQUAL(f.chunkName(2), "SSND");
    BOOST_CHECK_EQUAL(f.chunkPadding(2), 1);
    BOOST_CHECK_EQUAL(f.chunkOffset(3), 4408);
    BOOST_CHECK_EQUAL(f.chunkDataSize(3), 4);
    BOOST_CHECK_EQUAL(f.chunkName(3), "TEST");
    BOOST_CHECK_EQUAL(f.chunkPadding(3), 0);
    BOOST_CHECK_EQUAL(f.riffSize(), 4412 - 8);
  }
  {
    PublicRIFF f(copy.fileName());
    BOOST_CHECK_EQUAL(f.chunkOffset(2), 4088);
    BOOST_CHECK_EQUAL(f.chunkDataSize(2), 311);
    BOOST_CHECK_EQUAL(f.chunkName(2), "SSND");
    BOOST_CHECK_EQUAL(f.chunkPadding(2), 1);
    BOOST_CHECK_EQUAL(f.chunkOffset(3), 4408);
    BOOST_CHECK_EQUAL(f.chunkDataSize(3), 4);
    BOOST_CHECK_EQUAL(f.chunkName(3), "TEST");
    BOOST_CHECK_EQUAL(f.chunkPadding(3), 0);
    BOOST_CHECK_EQUAL(f.length(), 4412);
  }
}

BOOST_AUTO_TEST_CASE(testLastChunkAtEvenPosition2)
{
  const ScopedFileCopy copy("noise_odd", ".aif");
  {
    PublicRIFF f(copy.fileName());
    BOOST_CHECK_EQUAL(f.chunkOffset(2), 0xff0 + 8);
    BOOST_CHECK_EQUAL(f.chunkDataSize(2), 311);
    BOOST_CHECK_EQUAL(f.chunkName(2), "SSND");
    BOOST_CHECK_EQUAL(f.chunkPadding(2), 0);
    BOOST_CHECK_EQUAL(f.length(), 4399);
    BOOST_CHECK_EQUAL(f.riffSize(), 4399 - 8);
    f.setChunkData("TEST", "abcd");
    BOOST_CHECK_EQUAL(f.chunkOffset(2), 4088);
    BOOST_CHECK_EQUAL(f.chunkDataSize(2), 311);
    BOOST_CHECK_EQUAL(f.chunkName(2), "SSND");
    BOOST_CHECK_EQUAL(f.chunkPadding(2), 1);
    BOOST_CHECK_EQUAL(f.chunkOffset(3), 4408);
    BOOST_CHECK_EQUAL(f.chunkDataSize(3), 4);
    BOOST_CHECK_EQUAL(f.chunkName(3), "TEST");
    BOOST_CHECK_EQUAL(f.chunkPadding(3), 0);
    BOOST_CHECK_EQUAL(f.riffSize(), 4412 - 8);
  }
  {
    PublicRIFF f(copy.fileName());
    BOOST_CHECK_EQUAL(f.chunkOffset(2), 4088);
    BOOST_CHECK_EQUAL(f.chunkDataSize(2), 311);
    BOOST_CHECK_EQUAL(f.chunkName(2), "SSND");
    BOOST_CHECK_EQUAL(f.chunkPadding(2), 1);
    BOOST_CHECK_EQUAL(f.chunkOffset(3), 4408);
    BOOST_CHECK_EQUAL(f.chunkDataSize(3), 4);
    BOOST_CHECK_EQUAL(f.chunkName(3), "TEST");
    BOOST_CHECK_EQUAL(f.chunkPadding(3), 0);
    BOOST_CHECK_EQUAL(f.length(), 4412);
  }
}

BOOST_AUTO_TEST_CASE(testLastChunkAtEvenPosition3)
{
  const ScopedFileCopy copy("noise_odd", ".aif");
  {
    PublicRIFF f(copy.fileName());
    BOOST_CHECK_EQUAL(f.chunkOffset(2), 0xff0 + 8);
    BOOST_CHECK_EQUAL(f.chunkDataSize(2), 311);
    BOOST_CHECK_EQUAL(f.chunkName(2), "SSND");
    BOOST_CHECK_EQUAL(f.chunkPadding(2), 0);
    BOOST_CHECK_EQUAL(f.length(), 4399);
    BOOST_CHECK_EQUAL(f.riffSize(), 4399 - 8);
    f.setChunkData("TEST", "abc");
    BOOST_CHECK_EQUAL(f.chunkOffset(2), 4088);
    BOOST_CHECK_EQUAL(f.chunkDataSize(2), 311);
    BOOST_CHECK_EQUAL(f.chunkName(2), "SSND");
    BOOST_CHECK_EQUAL(f.chunkPadding(2), 1);
    BOOST_CHECK_EQUAL(f.chunkOffset(3), 4408);
    BOOST_CHECK_EQUAL(f.chunkDataSize(3), 3);
    BOOST_CHECK_EQUAL(f.chunkName(3), "TEST");
    BOOST_CHECK_EQUAL(f.chunkPadding(3), 1);
    BOOST_CHECK_EQUAL(f.riffSize(), 4412 - 8);
  }
  {
    PublicRIFF f(copy.fileName());
    BOOST_CHECK_EQUAL(f.chunkOffset(2), 4088);
    BOOST_CHECK_EQUAL(f.chunkDataSize(2), 311);
    BOOST_CHECK_EQUAL(f.chunkName(2), "SSND");
    BOOST_CHECK_EQUAL(f.chunkPadding(2), 1);
    BOOST_CHECK_EQUAL(f.chunkOffset(3), 4408);
    BOOST_CHECK_EQUAL(f.chunkDataSize(3), 3);
    BOOST_CHECK_EQUAL(f.chunkName(3), "TEST");
    BOOST_CHECK_EQUAL(f.chunkPadding(3), 1);
    BOOST_CHECK_EQUAL(f.length(), 4412);
  }
}

BOOST_AUTO_TEST_CASE(testChunkOffset)
{
  const ScopedFileCopy copy("empty", ".aiff");
  
  PublicRIFF f(copy.fileName());

  BOOST_CHECK_EQUAL(f.riffSize(), 5928);
  BOOST_CHECK_EQUAL(f.length(), 5936);
  BOOST_CHECK_EQUAL(f.chunkName(0), "COMM");
  BOOST_CHECK_EQUAL(f.chunkOffset(0), 0x000C + 8);
  BOOST_CHECK_EQUAL(f.chunkName(1), "SSND");
  BOOST_CHECK_EQUAL(f.chunkOffset(1), 0x0026 + 8);
  BOOST_CHECK_EQUAL(f.chunkName(2), "TEST");
  BOOST_CHECK_EQUAL(f.chunkOffset(2), 0x1728 + 8);
  
  const ByteVector data(0x400, ' ');
  f.setChunkData("SSND", data);
  BOOST_CHECK_EQUAL(f.riffSize(), 1070);
  BOOST_CHECK_EQUAL(f.length(), 1078);
  BOOST_CHECK_EQUAL(f.chunkOffset(0), 0x000C + 8);
  BOOST_CHECK_EQUAL(f.chunkOffset(1), 0x0026 + 8);
  BOOST_CHECK_EQUAL(f.chunkOffset(2), 0x042E + 8);
  
  f.seek(f.chunkOffset(0) - 8);
  BOOST_CHECK_EQUAL(f.readBlock(4), "COMM");
  f.seek(f.chunkOffset(1) - 8);
  BOOST_CHECK_EQUAL(f.readBlock(4), "SSND");
  f.seek(f.chunkOffset(2) - 8);
  BOOST_CHECK_EQUAL(f.readBlock(4), "TEST");
  
  f.setChunkData(0, data);
  BOOST_CHECK_EQUAL(f.riffSize(), 2076);
  BOOST_CHECK_EQUAL(f.length(), 2084);
  BOOST_CHECK_EQUAL(f.chunkOffset(0), 0x000C + 8);
  BOOST_CHECK_EQUAL(f.chunkOffset(1), 0x0414 + 8);
  BOOST_CHECK_EQUAL(f.chunkOffset(2), 0x081C + 8);
  
  f.seek(f.chunkOffset(0) - 8);
  BOOST_CHECK_EQUAL(f.readBlock(4), "COMM");
  f.seek(f.chunkOffset(1) - 8);
  BOOST_CHECK_EQUAL(f.readBlock(4), "SSND");
  f.seek(f.chunkOffset(2) - 8);
  BOOST_CHECK_EQUAL(f.readBlock(4), "TEST");
  
  f.removeChunk("SSND");
  BOOST_CHECK_EQUAL(f.riffSize(), 1044);
  BOOST_CHECK_EQUAL(f.length(), 1052);
  BOOST_CHECK_EQUAL(f.chunkOffset(0), 0x000C + 8);
  BOOST_CHECK_EQUAL(f.chunkOffset(1), 0x0414 + 8);
  
  f.seek(f.chunkOffset(0) - 8);
  BOOST_CHECK_EQUAL(f.readBlock(4), "COMM");
  f.seek(f.chunkOffset(1) - 8);
  BOOST_CHECK_EQUAL(f.readBlock(4), "TEST");
  
  f.removeChunk(0);
  BOOST_CHECK_EQUAL(f.riffSize(), 12);
  BOOST_CHECK_EQUAL(f.length(), 20);
  BOOST_CHECK_EQUAL(f.chunkOffset(0), 0x000C + 8);
  
  f.seek(f.chunkOffset(0) - 8);
  BOOST_CHECK_EQUAL(f.readBlock(4), "TEST");
}

BOOST_AUTO_TEST_SUITE_END()
