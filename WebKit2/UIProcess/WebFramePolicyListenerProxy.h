/*
 * Copyright (C) 2010 Apple Inc. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY APPLE INC. AND ITS CONTRIBUTORS ``AS IS''
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL APPLE INC. OR ITS CONTRIBUTORS
 * BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF
 * THE POSSIBILITY OF SUCH DAMAGE.
 */

#pragma once

#include "WebFrameListenerProxy.h"

#if PLATFORM(COCOA)
#include "WKFoundation.h"
#endif

#define DELEGATE_REF_COUNTING_TO_COCOA (PLATFORM(COCOA) && WK_API_ENABLED)

namespace WebKit {

enum class PolicyListenerType {
    NavigationAction,
    NewWindowAction,
    Response,
};

class WebFramePolicyListenerProxy : public WebFrameListenerProxy {
public:
    static const Type APIType = Type::FramePolicyListener;

    static Ref<WebFramePolicyListenerProxy> create(WebFrameProxy* frame, uint64_t listenerID, PolicyListenerType policyType)
    {
        return adoptRef(*new WebFramePolicyListenerProxy(frame, listenerID, policyType));
    }

    void use(std::optional<WebsitePoliciesData>&&);
    void download();
    void ignore();

    PolicyListenerType policyListenerType() const { return m_policyType; }

private:
    WebFramePolicyListenerProxy(WebFrameProxy*, uint64_t listenerID, PolicyListenerType);

    Type type() const override { return APIType; }

#if DELEGATE_REF_COUNTING_TO_COCOA
    void* operator new(size_t size) { return newObject(size, APIType); }
#endif

    PolicyListenerType m_policyType;
};

} // namespace WebKit

#undef DELEGATE_REF_COUNTING_TO_COCOA
