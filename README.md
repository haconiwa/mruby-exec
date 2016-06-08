# mruby-exec   [![Build Status](https://travis-ci.org/udzura/mruby-exec.svg?branch=master)](https://travis-ci.org/udzura/mruby-exec)
Exec class
## install by mrbgems
- add conf.gem line to `build_config.rb`

```ruby
MRuby::Build.new do |conf|

    # ... (snip) ...

    conf.gem :github => 'udzura/mruby-exec'
end
```
## example
```ruby
p Exec.hi
#=> "hi!!"
t = Exec.new "hello"
p t.hello
#=> "hello"
p t.bye
#=> "hello bye"
```

## License
under the MIT License:
- see LICENSE file
