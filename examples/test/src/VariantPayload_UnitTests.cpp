// VariantPayload_UnitTests.cpp

#include <SDOM/SDOM_VariantPayload.hpp>
#include <SDOM/SDOM_UnitTests.hpp>
#include <type_traits>

namespace SDOM {

namespace {

static_assert(std::is_const_v<std::remove_pointer_t<decltype(std::declval<const VariantPayload&>().array())>>,
              "VariantPayload::array must remain read-only");
static_assert(std::is_const_v<std::remove_pointer_t<decltype(std::declval<const VariantPayload&>().object())>>,
              "VariantPayload::object must remain read-only");

bool VariantPayload_test_scalars(std::vector<std::string>& errors) {
    VariantPayload nullPayload;
    if (!nullPayload.isNull()) errors.push_back("Default VariantPayload not null");
    if (nullPayload.type() != VariantType::Null) errors.push_back("Null type mismatch");

    VariantPayload boolPayload(true);
    if (!boolPayload.isBool()) errors.push_back("Bool payload not detected as bool");
    if (!boolPayload.toBool()) errors.push_back("Bool payload toBool failed");

    VariantPayload intPayload(int64_t(42));
    if (!intPayload.isInt()) errors.push_back("Int payload not detected as int");
    if (intPayload.toInt64() != 42) errors.push_back("Int payload toInt mismatch");

    VariantPayload doublePayload(3.5);
    if (!doublePayload.isReal()) errors.push_back("Real payload not detected as real");
    if (doublePayload.toDouble() != 3.5) errors.push_back("Real payload toDouble mismatch");

    VariantPayload stringPayload("hello");
    if (!stringPayload.isString()) errors.push_back("String payload not detected as string");
    if (stringPayload.toString() != "hello") errors.push_back("String payload toString mismatch");

    VariantPayload errorPayload = VariantPayload::makeError("boom");
    if (!errorPayload.isError()) errors.push_back("Error payload missing error flag");
    if (errorPayload.errorMessage() != "boom") errors.push_back("Error payload message mismatch");

    return true;
}

bool VariantPayload_test_array_object(std::vector<std::string>& errors) {
    VariantPayload::Array arr;
    arr.push_back(std::make_shared<VariantPayload>(VariantPayload(1)));
    arr.push_back(std::make_shared<VariantPayload>(VariantPayload("two")));
    VariantPayload arrayPayload(std::move(arr));
    if (!arrayPayload.isArray()) errors.push_back("Array ctor did not mark array type");
    const auto* arrView = arrayPayload.array();
    if (!arrView || arrView->size() != 2) errors.push_back("Array size mismatch");
    else {
        if (!(*arrView)[0] || !(*arrView)[0]->isInt()) errors.push_back("Array element 0 missing/int mismatch");
        if (!(*arrView)[1] || !(*arrView)[1]->isString()) errors.push_back("Array element 1 missing/string mismatch");
    }

    VariantPayload::Object objMap;
    objMap["answer"] = std::make_shared<VariantPayload>(VariantPayload(42));
    objMap["label"] = std::make_shared<VariantPayload>(VariantPayload("forty-two"));
    VariantPayload objectPayload(std::move(objMap));
    if (!objectPayload.isObject()) errors.push_back("Object ctor did not mark object type");
    const auto* objView = objectPayload.object();
    if (!objView || objView->size() != 2) errors.push_back("Object size mismatch");
    else {
        auto answerIt = objView->find("answer");
        auto labelIt = objView->find("label");
        if (answerIt == objView->end() || !answerIt->second || !answerIt->second->isInt())
            errors.push_back("Object 'answer' missing");
        if (labelIt == objView->end() || !labelIt->second || !labelIt->second->isString())
            errors.push_back("Object 'label' missing");
    }

    return true;
}

bool VariantPayload_test_json_roundtrip(std::vector<std::string>& errors) {
    nlohmann::json reference = {
        {"flag", true},
        {"value", 123},
        {"items", {1, 2, 3}},
        {"inner", {{"name", "payload"}, {"count", 2}}}
    };

    VariantPayload payload = VariantPayload::fromJson(reference);
    if (!payload.isObject()) errors.push_back("fromJson did not produce object payload");

    nlohmann::json roundtrip = payload.toJson();
    if (roundtrip != reference) errors.push_back("toJson roundtrip mismatch");

    JsonConversionOptions opts;
    opts.max_depth = 1;
    VariantPayload depthPayload = VariantPayload::fromJson(reference, opts);
    if (!depthPayload.hasError()) errors.push_back("Depth guard should trigger error");

    return true;
}

bool VariantPayload_test_value_semantics(std::vector<std::string>& errors) {
    auto sharedChild = std::make_shared<VariantPayload>(VariantPayload(99));
    std::weak_ptr<VariantPayload> childWeak = sharedChild;

    VariantPayload::Array arr;
    arr.push_back(sharedChild);
    sharedChild.reset();

    VariantPayload original(std::move(arr));
    VariantPayload copy = original;
    VariantPayload moved = std::move(original);

    const auto* copyArr = copy.array();
    const auto* movedArr = moved.array();
    auto locked = childWeak.lock();
    if (!locked) errors.push_back("Child expired after copies");

    if (!copyArr || !movedArr) errors.push_back("Copy/move lost array view");
    else if (!(*copyArr)[0] || !(*movedArr)[0])
        errors.push_back("Copy/move lost child node");
    else {
        if ((*copyArr)[0].get() != locked.get()) errors.push_back("Copy created new child");
        if ((*movedArr)[0].get() != locked.get()) errors.push_back("Move created new child");
    }

    moved = VariantPayload(); // drop remaining owner
    if (childWeak.expired()) errors.push_back("Shared child destroyed prematurely");

    return true;
}

bool VariantPayload_test_readonly_contract(std::vector<std::string>& errors) {
    VariantPayload::Array arr;
    arr.push_back(std::make_shared<VariantPayload>(VariantPayload(5)));
    VariantPayload payload(std::move(arr));

    const auto* before = payload.array();
    auto beforePtr = before && !before->empty() ? (*before)[0].get() : nullptr;
    payload.toJson(); // should be read-only
    const auto* after = payload.array();
    auto afterPtr = after && !after->empty() ? (*after)[0].get() : nullptr;

    if (before != after) errors.push_back("array() pointer changed after toJson");
    if (beforePtr != afterPtr) errors.push_back("Child pointer changed after toJson");

    return true;
}

bool VariantPayload_test_type_deduction(std::vector<std::string>& errors) {
    struct Case {
        VariantPayload payload;
        VariantType expected;
    };

    std::vector<Case> cases;
    cases.push_back({VariantPayload(), VariantType::Null});
    cases.push_back({VariantPayload(true), VariantType::Bool});
    cases.push_back({VariantPayload(int64_t(7)), VariantType::Int});
    cases.push_back({VariantPayload(3.5), VariantType::Real});
    cases.push_back({VariantPayload("txt"), VariantType::String});

    VariantPayload::Array arr;
    arr.push_back(std::make_shared<VariantPayload>(VariantPayload(1)));
    cases.push_back({VariantPayload(arr), VariantType::Array});

    VariantPayload::Object obj;
    obj["k"] = std::make_shared<VariantPayload>(VariantPayload("v"));
    cases.push_back({VariantPayload(obj), VariantType::Object});

    auto dyn = std::make_shared<int>(3);
    cases.push_back({VariantPayload(dyn, typeid(int), "int"), VariantType::Dynamic});

    Event* fakeEvent = reinterpret_cast<Event*>(0x1);
    cases.push_back({VariantPayload(fakeEvent), VariantType::Event});

    cases.push_back({VariantPayload::makeError("err"), VariantType::Error});

    for (const auto& c : cases) {
        if (c.payload.type() != c.expected) {
            errors.push_back("VariantPayload::type mismatch");
        }
    }

    return true;
}

bool VariantPayload_test_structural_sharing(std::vector<std::string>& errors) {
    auto child = std::make_shared<VariantPayload>(VariantPayload("child"));
    std::weak_ptr<VariantPayload> weak = child;
    VariantPayload::Array arr;
    arr.push_back(child);
    child.reset();

    VariantPayload payload(std::move(arr));
    VariantPayload copy = payload;

    payload = VariantPayload(); // drop first owner

    if (weak.expired()) errors.push_back("Child expired after parent reset");
    auto locked = weak.lock();
    const auto* copyArr = copy.array();
    if (!copyArr || copyArr->empty() || !(*copyArr)[0])
        errors.push_back("Copy lost child entry");
    else if ((*copyArr)[0].get() != locked.get())
        errors.push_back("Copied payload did not share child");

    return true;
}

bool VariantPayload_test_error_payload(std::vector<std::string>& errors) {
    VariantPayload payload = VariantPayload::makeError("intentional");
    if (!payload.hasError()) errors.push_back("Error payload missing flag");
    if (payload.type() != VariantType::Error) errors.push_back("Error payload type mismatch");
    if (payload.errorMessage() != "intentional") errors.push_back("Error payload message mismatch");
    if (!payload.toJson().is_null()) errors.push_back("Error payload toJson should be null");
    return true;
}

bool VariantPayload_test_event_serialization(std::vector<std::string>& errors) {
    Event* fakeEvent = reinterpret_cast<Event*>(0x2);
    VariantPayload payload(fakeEvent);
    if (!payload.isEvent()) errors.push_back("Event payload should report Event type");
    nlohmann::json out = payload.toJson();
    if (!out.is_null()) errors.push_back("Event payload toJson should return null placeholder");
    return true;
}

bool VariantPayload_test_dynamic_value_contract(std::vector<std::string>& errors) {
    struct Dummy { int value = 5; };
    auto dummy = std::make_shared<Dummy>();
    std::weak_ptr<Dummy> weak = dummy;

    VariantPayload payload(dummy, typeid(Dummy), "Dummy");
    nlohmann::json out = payload.toJson();
    if (!out.is_null()) errors.push_back("Dynamic payload toJson should fall back to null without converter");
    if (weak.expired()) errors.push_back("Dynamic payload should not destroy shared_ptr during toJson");
    return true;
}

} // namespace

bool VariantPayload_UnitTests() {
    static bool registered = false;
    UnitTests& ut = UnitTests::getInstance();
    const std::string objName = "VariantPayload";

    if (!registered) {
        ut.add_test(objName, "Scalars & errors", VariantPayload_test_scalars);
        ut.add_test(objName, "Array/Object storage", VariantPayload_test_array_object);
        ut.add_test(objName, "JSON roundtrip", VariantPayload_test_json_roundtrip);
        ut.add_test(objName, "Value semantics", VariantPayload_test_value_semantics);
        ut.add_test(objName, "Read-only contract", VariantPayload_test_readonly_contract);
        ut.add_test(objName, "Type deduction", VariantPayload_test_type_deduction);
        ut.add_test(objName, "Structural sharing", VariantPayload_test_structural_sharing);
        ut.add_test(objName, "Error payload behavior", VariantPayload_test_error_payload);
        ut.add_test(objName, "Event serialization", VariantPayload_test_event_serialization);
        ut.add_test(objName, "Dynamic value contract", VariantPayload_test_dynamic_value_contract);
        registered = true;
    }

    return true;
}

} // namespace SDOM
