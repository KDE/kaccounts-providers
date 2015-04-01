file(GLOB provider_in_files *.provider.in)
foreach(provider_in_file ${provider_in_files})
    kaccounts_add_provider(${provider_in_file})
endforeach()
