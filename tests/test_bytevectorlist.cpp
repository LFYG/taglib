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

#include <tbytevector.h>
#include <tbytevectorlist.h>
#include <boost/test/unit_test.hpp>
#include "loghelpers.h"

using namespace TagLib;

BOOST_AUTO_TEST_SUITE(TestByteVectorList)

BOOST_AUTO_TEST_CASE(testSplitSingleChar)
{
  const ByteVector v("a b");
  const ByteVectorList l = ByteVectorList::split(v, " ");
  BOOST_CHECK_EQUAL(l.size(), 2);
  BOOST_CHECK_EQUAL(l[0], "a");
  BOOST_CHECK_EQUAL(l[1], "b");
}

BOOST_AUTO_TEST_CASE(testSplitSingleChar_2)
{
  const ByteVector v("a");
  const ByteVectorList l = ByteVectorList::split(v, " ");
  BOOST_CHECK_EQUAL(l.size(), 1);
  BOOST_CHECK_EQUAL(l[0], "a");
}

BOOST_AUTO_TEST_SUITE_END()
