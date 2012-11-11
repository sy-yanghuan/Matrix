/*
 * link_list.hpp
 *
 *  Created on: 2012-5-24
 *      Author: YH
 */

#ifndef LINK_LIST_HPP_
#define LINK_LIST_HPP_

#include <boost/intrusive/list.hpp>

namespace matrix {
namespace base {

namespace itsve = boost::intrusive;

typedef itsve::list_member_hook<itsve::link_mode<itsve::auto_unlink>> LinkListHook;

template<typename T, LinkListHook T::*PtrToMember>
class LinkList: public itsve::list<T, itsve::member_hook<T, LinkListHook, PtrToMember>,
        itsve::constant_time_size<false>> {
};

typedef itsve::list_member_hook<itsve::link_mode<itsve::safe_link>> CountedLinkListHook;

template<typename T, CountedLinkListHook T::* PtrToMember>
class CountedLinkList: public itsve::list<T,
        itsve::member_hook<T, CountedLinkListHook, PtrToMember>, itsve::constant_time_size<true> > {
};

}  // namespace base
}  // namespace matrix



#endif /* LINK_LIST_HPP_ */
