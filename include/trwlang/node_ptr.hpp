#pragma once

#include <trwlang/node.hpp>

#include <atomic>
#include <utility>

namespace trwlang {

struct node_ptr {

  node_ptr () noexcept
  : ptr_(nullptr) {
  }

  node_ptr (node * ptr) noexcept
  : ptr_(ptr) {
    if (ptr_) { acquire_count(); }
  }

  node_ptr (node_ptr const & other) noexcept
  : ptr_(other.ptr_) {
    if (ptr_) { acquire_count(); }
  }

  node_ptr (node_ptr && other) noexcept
  : ptr_(other.ptr_) {
    other.ptr_ = nullptr;
  }

  node_ptr & operator= (node_ptr const & other) noexcept {
    release(other);
    return *this;
  }

  node_ptr & operator= (node_ptr && other) noexcept {
    release(std::move(other));
    return *this;
  }

  void release () noexcept {
    if (ptr_ == nullptr) { return; }
    if (ptr_->refcount_.fetch_sub(1) == 1) {
      delete ptr_;
    }
    ptr_ = nullptr;
  }

  void release (node_ptr const & other) noexcept {
    release();
    ptr_ = other.ptr_;
    if (ptr_) { acquire_count(); }
  }

  void release (node_ptr && other) noexcept {
    release();
    ptr_ = other.ptr_;
    other.ptr_ = nullptr;
  }

  void release (node * ptr) noexcept {
    release();
    ptr_ = ptr;
    if (ptr_) { acquire_count(); }
  }

  node * get () const noexcept {
    return const_cast<node *>(ptr_);
  }

  bool operator== (node_ptr const & other) const noexcept {
    return ptr_ == other.ptr_;
  }

  bool operator!= (node_ptr const & other) const noexcept {
    return ptr_ != other.ptr_;
  }

  node * operator-> () const noexcept {
    return const_cast<node *>(ptr_);
  }

  node & operator* () const noexcept {
    return const_cast<node &>(*ptr_);
  }

  operator bool () const noexcept {
    return ptr_ != nullptr;
  }

  ~node_ptr () noexcept {
    release();
  }

private:
  node * ptr_;

  void acquire_count () noexcept {
    ++(ptr_->refcount_);
  }

  void release_count () noexcept {
    --(ptr_->refcount_);
  }
};

}
