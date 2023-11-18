#ifndef VARIABLE_HPP
#define VARIABLE_HPP

#include <any>

namespace variable {

template <typename T>
struct ShouldMove {
  // NOLINTNEXTLINE
  static constexpr bool value = std::is_same_v<T, std::remove_reference_t<T>>;
};

template <typename T, typename U>
constexpr decltype(auto) MoveIfRr(U&& value) {
  if constexpr (ShouldMove<T>::value) {
    return std::move(value);
  } else {
    return value;
  }
}

template <typename... Types>
struct TypeList {};

template <typename Scope, std::size_t N>
struct Flag {
  friend constexpr auto Injector(Flag<Scope, N>);
};

template <typename T = void, auto W = [] {}>
struct UniqueT {};

template <typename Scope, std::size_t N, typename T>
struct Writer {
  friend constexpr auto Injector(Flag<Scope, N>) {
    return TypeList<T>{};
  }
};

template <typename Scope, std::size_t N = 1, auto = [] {}>
consteval auto Reader() {
  if constexpr (requires { Injector(Flag<Scope, N>{}); }) {
    return Reader<Scope, N + 1>();
  } else {
    return N - 1;
  }
}

template <typename T, typename... Types>
auto PushBack(TypeList<Types...>) {
  return TypeList<Types..., T>{};
}

template <typename T, typename... Types>
auto Front(TypeList<T, Types...>) -> T;

template <std::size_t N, typename T, typename... Types>
auto At(TypeList<T, Types...>) {
  static_assert(N <= sizeof...(Types));
  if constexpr (N == 0) {
    return TypeList<T>{};
  } else {
    return TypeList<decltype(At<N - 1>(TypeList<Types...>{}))>{};
  }
}

template <typename... Types>
auto Back(TypeList<Types...> x) -> decltype(Front(At<sizeof...(Types) - 1>(x)));

template <typename... Types>
std::size_t Size(TypeList<Types...>) {
  return sizeof...(Types);
}

struct VarTag {};

template <auto>
struct Var;

template <auto W, typename T, typename U>
decltype(auto) UnvarForward(U&& arg) {
  if constexpr (std::is_base_of_v<VarTag, std::remove_reference_t<T>>) {
    return MoveIfRr<T>(arg.template Get<W>());
  } else {
    return std::forward<T>(arg);
  }
}

template <auto Scope, typename T>
auto ScopedDeclval() -> T;

template <auto W, typename T>
auto Unvar() {
  if constexpr (std::is_base_of_v<VarTag, std::remove_reference_t<T>>) {
    return std::type_identity<
        decltype(ScopedDeclval<W, T>().template Get<W>())>{};
  } else {
    return std::type_identity<std::decay_t<T>>{};
  }
}

template <auto W, typename T>
using UnvarT = decltype(Unvar<W, T>())::type;

template <typename T, typename Scope, auto W>
auto TypeWriter() {
  if constexpr (Reader<Scope, 1, W>() == 0) {
    (void)Writer<Scope, 1, T>{};
  } else {
    (void)Writer<Scope, Reader<Scope, 1, W>() + 1, T>{};
  }
  return 0;
}

template <auto VarID = [] {}>
struct Var : VarTag {
  using Scope = UniqueT<void, VarID>;

  template <typename T, auto W = [] {}>
  auto& operator=(T&& rhs) {
    (void)TypeWriter<UnvarT<W, T>, Scope, W>();
    value.emplace<UnvarT<W, T>>(UnvarForward<W, T>(rhs));
    return *this;
  }

  template <typename T, auto W = [] {},
            typename = decltype(TypeWriter<UnvarT<W, T>, Scope, W>())>
  Var(T&& rhs) {  // NOLINT
    value.emplace<UnvarT<W, T>>(UnvarForward<W, T>(rhs));
  }

  template <auto W = [] {}>
  auto operator->() {
    using LastT
        = decltype(Back(Injector(Flag<Scope, Reader<Scope, 1, W>()>{})));
    return std::any_cast<LastT>(&value);
  }

  template <auto W = [] {}>
  decltype(auto) Get() {
    using LastT
        = decltype(Back(Injector(Flag<Scope, Reader<Scope, 1, W>()>{})));
    return std::any_cast<LastT&>(value);
  }

  template <auto W = [] {}>
  decltype(auto) Get() const {
    using LastT
        = decltype(Back(Injector(Flag<Scope, Reader<Scope, 1, W>()>{})));
    return std::any_cast<const LastT&>(value);
  }

  template <auto W = [] {}>
      // NOLINTNEXTLINE
      operator decltype(Back(Injector(Flag<Scope, Reader<Scope, 1, W>()>{})))
      & () {
    using LastT
        = decltype(Back(Injector(Flag<Scope, Reader<Scope, 1, W>()>{})));
    return std::any_cast<LastT&>(value);
  }

  template <auto W = [] {}>
  decltype(auto) operator++(int) {
    return Get<W>()++;
  }

  template <auto W = [] {}>
  decltype(auto) operator++() {
    return ++Get<W>();
  }

  template <auto W = [] {}>
  decltype(auto) operator--(int) {
    return Get<W>()--;
  }

  template <auto W = [] {}>
  decltype(auto) operator--() {
    return --Get<W>();
  }

  template <auto W = [] {}, typename... Args>
  decltype(auto) operator[](Args&&... args) {
    return Get<W>().operator[](std::forward<Args>(args)...);
  }

  template <auto W = [] {}, typename... Args>
  decltype(auto) operator()(Args&&... args) {
    return (Get<W>())(std::forward<Args>(args)...);
  }

  template <auto W = [] {}>
  decltype(auto) operator*() {
    return *(Get<W>());
  }

  template <auto W = [] {}, typename T>
  decltype(auto) operator<(T&& arg) {
    return Get<W>() < UnvarForward<W, T>(arg);
  }

  template <auto W = [] {}, typename T>
  decltype(auto) operator>(T&& arg) {
    return Get<W>() > UnvarForward<W, T>(arg);
  }

  template <auto W = [] {}, typename T>
  decltype(auto) operator<=(T&& arg) {
    return Get<W>() <= UnvarForward<W, T>(arg);
  }

  template <auto W = [] {}, typename T>
  decltype(auto) operator>=(T&& arg) {
    return Get<W>() >= UnvarForward<W, T>(arg);
  }

  template <auto W = [] {}, typename T>
  decltype(auto) operator==(T&& arg) {
    return Get<W>() == UnvarForward<W, T>(arg);
  }

  template <auto W = [] {}, typename T>
  decltype(auto) operator!=(T&& arg) {
    return Get<W>() != UnvarForward<W, T>(arg);
  }

  template <auto W = [] {}, typename T>
  decltype(auto) operator<=>(T&& arg) {
    return Get<W>() <=> UnvarForward<W, T>(arg);
  }

  template <auto W = [] {}, typename T>
  decltype(auto) operator+(T&& arg) {
    return Get<W>() + UnvarForward<W, T>(arg);
  }

  template <auto W = [] {}, typename T>
  decltype(auto) operator-(T&& arg) {
    return Get<W>() - UnvarForward<W, T>(arg);
  }

  template <auto W = [] {}, typename T>
  decltype(auto) operator+=(T&& arg) {
    return Get<W>() += UnvarForward<W, T>(arg);
  }

  template <auto W = [] {}, typename T>
  decltype(auto) operator-=(T&& arg) {
    return Get<W>() -= UnvarForward<W, T>(arg);
  }

  template <auto W = [] {}, typename T>
  decltype(auto) operator*(T&& arg) {
    return Get<W>() * UnvarForward<W, T>(arg);
  }

  template <auto W = [] {}, typename T>
  decltype(auto) operator*=(T&& arg) {
    return Get<W>() *= UnvarForward<W, T>(arg);
  }

  template <auto W = [] {}, typename T>
  decltype(auto) operator/=(T&& arg) {
    return Get<W>() /= UnvarForward<W, T>(arg);
  }

  template <auto W = [] {}>
  decltype(auto) begin() {  // NOLINT
    return Get<W>().begin();
  }

  template <auto W = [] {}>
  decltype(auto) end() {  // NOLINT
    return Get<W>().end();
  }

  std::any value;
};

}  // namespace variable

#endif