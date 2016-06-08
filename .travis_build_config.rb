MRuby::Build.new do |conf|
  toolchain :gcc
  conf.gembox 'default'
  conf.gem '../mruby-exec'
  conf.enable_test
end
