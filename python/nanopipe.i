%module nanopipe_mod
%{
#include <nanopipe/nanopipe.hpp>
%}

%include <std_string.i>

%include <nanopipe/nanopipe.hpp>

%include "producerconsumer.hpp"
%include "nanosched.hpp"
%include "nanomessage.hpp"
%include "nanosend.hpp"
%include "nanosend.hpp"
%include "nanosource.hpp"
%include "nanosource.hpp"
%include "redissource.hpp"
%include "streamsend.hpp"
%include "redissend.hpp"
%include "mqttsend.hpp"
%include "mqttsource.hpp"
%include "amqpsource.hpp"
%include "amqpsend.hpp"
%include "websocketsource.hpp"
%include "websocketsend.hpp"

