set(CMAKE_MESSAGE_CONTEXT_SHOW, TRUE)

function(GETSUBDIRS ret in add_in)
    set(dirs)
    if(add_in)
        list(APPEND dirs ${in})
    endif() 

    file(GLOB_RECURSE children LIST_DIRECTORIES true ${in}/*)  

    foreach(child ${children})
        if(IS_DIRECTORY ${child})
            list(APPEND dirs ${child})    
        endif()
    endforeach()    

    SET(${ret} ${dirs} PARENT_SCOPE)
endfunction()