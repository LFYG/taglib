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

#include <id3v1tag.h>
#include <id3v1genres.h>
#include <mpegfile.h>
#include <boost/test/unit_test.hpp>
#include "utils.h"
#include "loghelpers.h"

using namespace TagLib;

BOOST_AUTO_TEST_SUITE(TestID3v1)

BOOST_AUTO_TEST_CASE(testStripWhiteSpace)
{
  const ScopedFileCopy copy("xing", ".mp3");
  {
    MPEG::File f(copy.fileName());
    f.ID3v1Tag(true)->setArtist("Artist     ");
    f.save();
  }
  
  {
    MPEG::File f(copy.fileName());
    BOOST_CHECK(f.ID3v1Tag(false));
    BOOST_CHECK_EQUAL(f.ID3v1Tag(false)->artist(), "Artist");
  }
}

BOOST_AUTO_TEST_CASE(testGenres)
{
  BOOST_CHECK_EQUAL(ID3v1::genre(50), "Darkwave");
  BOOST_CHECK_EQUAL(ID3v1::genreIndex("Humour"), 100);
}

BOOST_AUTO_TEST_SUITE_END()
