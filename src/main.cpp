#if defined(USING_NODE_MODE)
    #include "node.cpp"
#elif defined(USING_GATEWAY_MODE)
    #include "gateway.cpp"
#else
    #include "example/blink_led.cpp"
#endif
