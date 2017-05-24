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

#include <flacunknownmetadatablock.h>
#include <boost/test/unit_test.hpp>
#include "loghelpers.h"

using namespace TagLib;

BOOST_AUTO_TEST_SUITE(TestFLACUnknownMetadataBlock)

BOOST_AUTO_TEST_CASE(testAccessors)
{
  const ByteVector data("abc\x01", 4);
  FLAC::UnknownMetadataBlock block(42, data);
  BOOST_CHECK_EQUAL(block.code(), 42);
  BOOST_CHECK_EQUAL(block.data(), data);
  BOOST_CHECK_EQUAL(block.render(), data);

  const ByteVector data2("xxx", 3);
  block.setCode(13);
  block.setData(data2);
  BOOST_CHECK_EQUAL(block.code(), 13);
  BOOST_CHECK_EQUAL(block.data(), data2);
  BOOST_CHECK_EQUAL(block.render(), data2);
}

BOOST_AUTO_TEST_SUITE_END()
