/***************************************************************************
    copyright           : (C) 2010 by Lukas Lalinsky
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

#include <flacpicture.h>
#include <boost/test/unit_test.hpp>
#include "loghelpers.h"

using namespace TagLib;

namespace 
{
  const unsigned char pictureData[] = { 
    0x00, 0x00, 0x00, 0x03, 0x00, 0x00, 0x00, 0x09, 
    0x69, 0x6D, 0x61, 0x67, 0x65, 0x2F, 0x70, 0x6E, 
    0x67, 0x00, 0x00, 0x00, 0x08, 0x41, 0x20, 0x70, 
    0x69, 0x78, 0x65, 0x6C, 0x2E, 0x00, 0x00, 0x00, 
    0x01, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 
    0x18, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
    0x96, 0x89, 0x50, 0x4E, 0x47, 0x0D, 0x0A, 0x1A, 
    0x0A, 0x00, 0x00, 0x00, 0x0D, 0x49, 0x48, 0x44, 
    0x52, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 
    0x01, 0x08, 0x02, 0x00, 0x00, 0x00, 0x90, 0x77, 
    0x53, 0xDE, 0x00, 0x00, 0x00, 0x09, 0x70, 0x48, 
    0x59, 0x73, 0x00, 0x00, 0x0B, 0x13, 0x00, 0x00, 
    0x0B, 0x13, 0x01, 0x00, 0x9A, 0x9C, 0x18, 0x00, 
    0x00, 0x00, 0x07, 0x74, 0x49, 0x4D, 0x45, 0x07, 
    0xD6, 0x0B, 0x1C, 0x0A, 0x36, 0x06, 0x08, 0x44, 
    0x3D, 0x32, 0x00, 0x00, 0x00, 0x1D, 0x74, 0x45, 
    0x58, 0x74, 0x43, 0x6F, 0x6D, 0x6D, 0x65, 0x6E, 
    0x74, 0x00, 0x43, 0x72, 0x65, 0x61, 0x74, 0x65, 
    0x64, 0x20, 0x77, 0x69, 0x74, 0x68, 0x20, 0x54, 
    0x68, 0x65, 0x20, 0x47, 0x49, 0x4D, 0x50, 0xEF, 
    0x64, 0x25, 0x6E, 0x00, 0x00, 0x00, 0x0C, 0x49, 
    0x44, 0x41, 0x54, 0x08, 0xD7, 0x63, 0xF8, 0xFF, 
    0xFF, 0x3F, 0x00, 0x05, 0xFE, 0x02, 0xFE, 0xDC, 
    0xCC, 0x59, 0xE7, 0x00, 0x00, 0x00, 0x00, 0x49, 
    0x45, 0x4E, 0x44, 0xAE, 0x42, 0x60, 0x82 };
}

BOOST_AUTO_TEST_SUITE(TestFLACPicture)

BOOST_AUTO_TEST_CASE(testParse)
{
  const char *pdata = reinterpret_cast<const char*>(pictureData);
  
  FLAC::Picture pic(ByteVector(pdata, 199));
  
  BOOST_CHECK_EQUAL(int(pic.type()), 3);
  BOOST_CHECK_EQUAL(pic.width(), 1);
  BOOST_CHECK_EQUAL(pic.height(), 1);
  BOOST_CHECK_EQUAL(pic.colorDepth(), 24);
  BOOST_CHECK_EQUAL(pic.numColors(), 0);
  BOOST_CHECK_EQUAL(pic.mimeType(), "image/png");
  BOOST_CHECK_EQUAL(pic.description(), "A pixel.");
  BOOST_CHECK_EQUAL(pic.data().size(), 150);
}

BOOST_AUTO_TEST_CASE(testPassThrough)
{
  const char *pdata = reinterpret_cast<const char*>(pictureData);
  
  FLAC::Picture pic(ByteVector(pdata, 199));
  BOOST_CHECK_EQUAL(pic.render(), ByteVector(pdata, 199));
}

BOOST_AUTO_TEST_SUITE_END()
