/* Copyright (c) 2012 maidsafe.net limited
All rights reserved.

Redistribution and use in source and binary forms, with or without modification,
are permitted provided that the following conditions are met:

    * Redistributions of source code must retain the above copyright notice,
    this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright notice,
    this list of conditions and the following disclaimer in the documentation
    and/or other materials provided with the distribution.
    * Neither the name of the maidsafe.net limited nor the names of its
    contributors may be used to endorse or promote products derived from this
    software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED.  IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR
TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#ifndef MAIDSAFE_COMMON_ACTIVE_H_
#define MAIDSAFE_COMMON_ACTIVE_H_

#include <atomic>
#include <condition_variable>
#include <mutex>
#include <thread>
#include <functional>
#include <memory>

#include "boost/thread/thread.hpp"

#include "maidsafe/common/safe_queue.h"

namespace maidsafe {
class Active {
 public:
  typedef std::function<void()> Functor;
  Active() : done(false) {
    thread_ = std::unique_ptr<std::thread>(new std::thread( [=]{ this->Run(); } ) );
  }
  ~Active() {
    Send( [&]{ done = true; } );
    thread_->join();
  }
  void Send( Functor m ) { message_queue_.Push( m ); }

 private:
  Active( const Active& );
  void operator=( const Active& );
  bool done;
  SafeQueue<Functor> message_queue_;
  std::unique_ptr<std::thread> thread_;
  void Run() {
    while( !done ) {
      Functor msg;
      message_queue_.WaitAndPop(msg);
      msg();
    } // dtctr sets done to true
  }
};

}  // namespace maidsafe

#endif  // MAIDSAFE_COMMON_ACTIVE_H_
