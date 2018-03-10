MRuby::Gem::Specification.new('mruby-exec') do |spec|
  spec.license = 'MIT'
  spec.authors = 'Uchio Kondo'

  spec.add_test_dependency('mruby-process', :github => 'iij/mruby-process')
  spec.add_test_dependency('mruby-io', :github => 'iij/mruby-io')
  spec.add_test_dependency('mruby-errno', :github => 'iij/mruby-errno')
end
