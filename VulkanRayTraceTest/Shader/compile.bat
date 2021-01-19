C:/VulkanSDK/1.2.162.0/Bin32/glslc.exe shader.vert -o vert.spv
C:/VulkanSDK/1.2.162.0/Bin32/glslc.exe shader.frag -o frag.spv
C:/VulkanSDK/1.2.162.0/Bin32/glslc.exe --target-env=vulkan1.1 --target-spv=spv1.4 closesthit.rchit -o closesthit.rchit.spv
C:/VulkanSDK/1.2.162.0/Bin32/glslc.exe --target-env=vulkan1.1 --target-spv=spv1.4 raygen.rgen -o raygen.rgen.spv
C:/VulkanSDK/1.2.162.0/Bin32/glslc.exe --target-env=vulkan1.1 --target-spv=spv1.4 miss.rmiss -o miss.rmiss.spv
pause