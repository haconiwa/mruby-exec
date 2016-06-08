# mruby-exec   [![Build Status](https://travis-ci.org/haconiwa/mruby-exec.svg?branch=master)](https://travis-ci.org/haconiwa/mruby-exec)

Small `execve(2)` wrapper

## install by mrbgems

- add conf.gem line to `build_config.rb`

```ruby
MRuby::Build.new do |conf|

    # ... (snip) ...

    conf.gem :github => 'haconiwa/mruby-exec'
end
```

## example

```ruby
Exec.execve("/bin/bash")
#=> The process will become bash
```

## License

- under the MIT License: see [LICENSE](./LICENSE) file
