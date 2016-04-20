find . -name '*.cfg' -exec perl -p -i -e 's/^window_begin_time_relative .*/window_begin_time_relative 0.0/g' '{}' '+'
find . -name '*.cfg' -exec perl -p -i -e 's/^window_end_time_relative .*/window_end_time_relative 1.0/g' '{}' '+'
find . -name '*.cfg' -exec perl -p -i -e 's/^window_scale_relative .*/window_scale_relative 1.0/g' '{}' '+'

