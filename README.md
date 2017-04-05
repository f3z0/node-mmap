# node-mmap

mmap(2) bindings for node.js - stop slurping, start mapping.

*This project is forked from the original project at*
*https://github.com/bnoordhuis/node-mmap and has been updated*
*to work with node v6.9.2.*

##### What's changed?

* Migrated to node-gyp
* Introduced nan.h for future backwards compatibility (maybe?)
* Tested with electron, bridging the main/renderer divide.
* Only tested on Mac OS/x so far, please report any platform
  related issues 

## Installing

Through [npm](http://npmjs.org/):

	npm install https://github.com/f3z0/node-mmap.git

Or compile it from source with this one-liner:

	node-gyp configure build install

## Usage

    buffer = mmap.map(n_bytes, protection, flags, fd, offset);

<table>
  <tr>
    <td><i>n_bytes</i></td>
    <td>The number of bytes to map into memory.</td>
  </tr>
  <tr>
    <td><i>protection</i></td>
    <td>Memory protection: either <b>PROT_NONE</b> or a bitwise OR of <b>PROT_READ</b>, <b>PROT_WRITE</b> and <b>PROT_EXEC</b>.</td>
  </tr>
  <tr>
    <td><i>flags</i></td>
    <td>Flags: either <b>MAP_SHARED</b> or <b>MAP_PRIVATE</b>.</td>
  </tr>
  <tr>
    <td><i>fd</i></td>
    <td>File descriptor.</td>
  </tr>
  <tr>
    <td><i>offset</i></td>
    <td>File offset. Must be either zero or a multiple of <b>mmap.PAGESIZE</b>.</td>
  </tr>
</table>

See [the man page](http://www.opengroup.org/onlinepubs/000095399/functions/mmap.html) for more details.

## Example - Shared memory across two processes

##### First Process:
    const fs = require('fs');
    const mmap = require('node-mmap');
    
    var n_bytes = 1024;
    var fd = fs.openSync('/tmp/temp_file', 'w+');
    fs.writeSync(fd, '\0', n_bytes); // resize
    var buffer = mmap.map(n_bytes, mmap.PROT_READ | mmap.PROT_WRITE , mmap.MAP_SHARED, fd, 0);
    buffer.writeUInt32BE(0xDEADBEEF, 0);

##### Second Process:
    const fs = require('fs');
    const mmap = require('node-mmap');

    var fd = fs.openSync('/tmp/temp_file', 'r');
    var size = fs.fstatSync(fd).size;
    var buffer = mmap.map(size, mmap.PROT_READ, mmap.MAP_SHARED, fd, 0);
    console.log("I'll take dead beef? Answer: 0x" + buffer.readUInt32BE(0).toString(16));

The file is automatically unmapped when the buffer object is garbage collected.

## Bugs and limitations

* Specifying the memory address is not implemented. I couldn't think of a reason
  why you would want to do that from JavaScript. Convince me otherwise. :-)

* Reading from and writing to memory-mapped files is a potentially blocking
  operation. ~~*Do not* use this module in performance critical code. Better yet,
  don't use this module at all.~~ Be smart how you use this module, a single
  producer should write to memory and one or more consumers should read and
  these roles are not interchangeable. This is because we don't have semaphores
  nor mutexes in place to handle atomic memory writes (keep in mind memory writes
  pages at a time even if a single byte is changed). Why would you want to use
  mmap in production environment? Because shared memory is **the fastest type of
  inter-process communication possible**, but only if used correctly and with
  caution.

