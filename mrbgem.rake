MRuby::Gem::Specification.new('mruby-exec') do |spec|
  spec.license = 'MIT'
  spec.authors = 'Uchio Kondo'

  spec.add_test_dependency('mruby-process', :github => 'iij/mruby-process')
end
