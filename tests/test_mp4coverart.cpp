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

#include <mp4coverart.h>
#include <boost/test/unit_test.hpp>
#include "loghelpers.h"

using namespace TagLib;

BOOST_AUTO_TEST_SUITE(TestMP4CoverArt)

BOOST_AUTO_TEST_CASE(testSimple)
{
  MP4::CoverArt c(MP4::CoverArt::PNG, "foo");
  BOOST_CHECK_EQUAL(c.format(), MP4::CoverArt::PNG);
  BOOST_CHECK_EQUAL(c.data(), "foo");
  
  MP4::CoverArt c2(c);
  BOOST_CHECK_EQUAL(c2.format(), MP4::CoverArt::PNG);
  BOOST_CHECK_EQUAL(c2.data(), "foo");
  
  MP4::CoverArt c3 = c;
  BOOST_CHECK_EQUAL(c3.format(), MP4::CoverArt::PNG);
  BOOST_CHECK_EQUAL(c3.data(), "foo");
}

BOOST_AUTO_TEST_CASE(testList)
{
  MP4::CoverArtList l;
  l.append(MP4::CoverArt(MP4::CoverArt::PNG, "foo"));
  l.append(MP4::CoverArt(MP4::CoverArt::JPEG, "bar"));
  
  BOOST_CHECK_EQUAL(l[0].format(), MP4::CoverArt::PNG);
  BOOST_CHECK_EQUAL(l[0].data(), "foo");
  BOOST_CHECK_EQUAL(l[1].format(), MP4::CoverArt::JPEG);
  BOOST_CHECK_EQUAL(l[1].data(), "bar");
}

BOOST_AUTO_TEST_SUITE_END()
