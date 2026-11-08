"""Read failed Metal command buffers before SDL releases their fences in LLDB.

This debugger script inspects SDL 3.4's MetalFence, whose first member is the
retained MTLCommandBuffer. It never changes application or driver state.
"""

import lldb


def OnReleaseFence(frame, location, internal_dict):
    target = frame.GetThread().GetProcess().GetTarget()
    register = "x1" if "arm64" in target.GetTriple() else "rsi"
    fence = frame.FindRegister(register).GetValueAsUnsigned()
    if not fence:
        return False
    error = lldb.SBError()
    command = frame.GetThread().GetProcess().ReadPointerFromMemory(fence, error)
    if error.Fail() or not command:
        print("Metal fence inspection failed: " + str(error))
        return False
    options = lldb.SBExpressionOptions()
    options.SetTimeoutInMicroSeconds(250000)
    options.SetTryAllThreads(True)
    options.SetIgnoreBreakpoints(True)
    status = frame.EvaluateExpression("(long)[(id)0x%x status]" % command, options)
    if status.GetError().Fail():
        print("Metal command status inspection failed: " + str(status.GetError()))
        location.GetBreakpoint().SetEnabled(False)
    elif status.GetValueAsSigned() == 5:  # MTLCommandBufferStatusError
        failure = frame.EvaluateExpression(
            '(const char *)[[(id)[(id)0x%x error] localizedDescription] UTF8String]' % command,
            options)
        print("Metal command buffer error: " + str(failure.GetSummary()))
    return False


def __lldb_init_module(debugger, internal_dict):
    target = debugger.GetSelectedTarget()
    breakpoint = target.BreakpointCreateByName("SDL_ReleaseGPUFence")
    breakpoint.SetScriptCallbackFunction(__name__ + ".OnReleaseFence")
    print("Tracing Metal command buffer errors at SDL fence release")
