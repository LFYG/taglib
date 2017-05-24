/***************************************************************************
    copyright            : (C) 2017 by Tsuda Kageyu
    email                : tsuda.kageyu@gmail.com
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

namespace TagLib 
{
  // Helper functions that enables the test framework to print TagLib types.

#ifdef TAGLIB_STRING_H

  inline std::ostream &operator<<(std::ostream &s, const String &str)
  {
    return ::operator<<(s, str);
  }

#endif

#ifdef TAGLIB_BYTEVECTOR_H

  inline std::ostream &operator<<(std::ostream &s, const ByteVector &vec)
  {
    return ::operator<<(s, vec);
  }

#endif

#ifdef TAGLIB_LIST_H

  template <typename T>
  inline std::ostream &operator<<(std::ostream &s, const List<T> &l)
  {
    s << "{ ";
    for(typename List<T>::ConstIterator it = l.begin(); it != l.end(); ++it) {
      s << *it;
      if(it != --l.end())
        s << ", ";
    }
    s << " }";
    return s;
  }

#endif

}
