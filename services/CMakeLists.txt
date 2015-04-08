file(GLOB service_files_in *.service.in)
foreach(service_file_in ${service_files_in})
    kaccounts_add_service(${service_file_in})
endforeach()
