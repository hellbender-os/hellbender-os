        .global _signal_wrapper
        .extern _signal_processor
_signal_wrapper:
        // process the signal, restore state, and iret.
        call _signal_processor
        add $4*4, %esp // we have to pop the parameters to expose the state.
        pop %gs
        pop %fs
        pop %es
        pop %ds
        popal
        iret
        
