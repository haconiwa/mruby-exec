# mruby-exec   [![Build Status](https://travis-ci.org/haconiwa/mruby-exec.svg?branch=master)](https://travis-ci.org/haconiwa/mruby-exec)

Small `execv/execve` wrapper

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
Exec.execv("/bin/bash")
#=> The process will become bash

# Also you can pass more than 1 params
Exec.execv("/bin/bash", "-l", "-c", "echo Hello exec")
```

## License

- under the MIT License: see [LICENSE](./LICENSE) file
