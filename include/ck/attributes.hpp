#ifndef CK_ATTRIBUTES_HPP
#define CK_ATTRIBUTES_HPP

#include <ck/traits.hpp>

#define CK_ENTRY_ATTRIBUTE_DECLARE(name)        \
  template <auto Entry>                         \
  struct is_##name : public std::false_type {}; \
  template <auto Entry>                         \
  constexpr auto is_##name##_v = is_##name<Entry>::value;

#define CK_ENTRY_ATTRIBUTE_ASSIGN(name, class)        \
  namespace ck {                                      \
  template <>                                         \
  struct is_##class<name> : public std::true_type {}; \
  }

// can we condense this down to a one-liner?
#define CK_EXCLUSIVE_ENTRY(name) CK_ENTRY_ATTRIBUTE_ASSIGN(name, exclusive)
#define CK_EXPEDITED_ENTRY(name) CK_ENTRY_ATTRIBUTE_ASSIGN(name, expedited)
#define CK_IMMEDIATE_ENTRY(name) CK_ENTRY_ATTRIBUTE_ASSIGN(name, immediate)
#define CK_INLINE_ENTRY(name) CK_ENTRY_ATTRIBUTE_ASSIGN(name, inline)
#define CK_LOCAL_ENTRY(name) CK_ENTRY_ATTRIBUTE_ASSIGN(name, local)
#define CK_NOKEEP_ENTRY(name) CK_ENTRY_ATTRIBUTE_ASSIGN(name, nokeep)
#define CK_NOTRACE_ENTRY(name) CK_ENTRY_ATTRIBUTE_ASSIGN(name, notrace)
#define CK_THREADED_ENTRY(name) CK_ENTRY_ATTRIBUTE_ASSIGN(name, threaded)

namespace ck {

CK_ENTRY_ATTRIBUTE_DECLARE(exclusive);
CK_ENTRY_ATTRIBUTE_DECLARE(expedited);
CK_ENTRY_ATTRIBUTE_DECLARE(immediate);
CK_ENTRY_ATTRIBUTE_DECLARE(inline);
CK_ENTRY_ATTRIBUTE_DECLARE(local);
CK_ENTRY_ATTRIBUTE_DECLARE(notrace);
CK_ENTRY_ATTRIBUTE_DECLARE(threaded);

// TODO (add appwork, memcritcal, etc. )

template <auto Entry, typename Enable = void>
struct is_nokeep;

template <typename Class, typename... Args, member_fn_t<Class, Args...> Entry>
struct is_nokeep<Entry> {
  static constexpr auto value = !is_message_v<std::decay_t<Args>...>;
};

template <typename Class, typename Value, Value Class::*Entry>
struct is_nokeep<Entry, std::enable_if_t<!is_method_v<Entry>>> {
  static constexpr auto value = !is_message_v<typename Value::type>;
};

template <auto Entry>
constexpr auto is_nokeep_v = is_nokeep<Entry>::value;

namespace {
template <auto Entry>
constexpr auto registration_flags_impl(void) {
  auto flags = 0;
  flags |= CK_EP_INLINE * is_inline_v<Entry>;
  flags |= CK_EP_IMMEDIATE * is_immediate_v<Entry>;
  flags |= CK_EP_NOKEEP * is_nokeep_v<Entry>;
  flags |= CK_EP_TRACEDISABLE * is_notrace_v<Entry>;
  return flags;
}

template <auto Entry>
constexpr auto message_flags_impl(void) {
  auto flags = 0;
  flags |= CK_MSG_INLINE * is_inline_v<Entry>;
  flags |= CK_MSG_IMMEDIATE * is_immediate_v<Entry>;
  flags |= CK_MSG_EXPEDITED * is_expedited_v<Entry>;
  flags |= CK_MSG_LB_NOTRACE * is_notrace_v<Entry>;
  return flags;
}
}  // namespace

template <auto Entry>
constexpr auto message_flags = message_flags_impl<Entry>();

template <auto Entry>
constexpr auto registration_flags = registration_flags_impl<Entry>();
}  // namespace ck

#endif
