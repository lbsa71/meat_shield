# Pinned editor automation

Status: Accepted for development setup, 1 November 2026. DEC-20; REQ-019; MS-001. M0 gameplay and packaged-build acceptance remains open.

The product owner selected Unreal and Blender, AI-led implementation, and procedural authoring primarily in Unreal. Use the regular Unreal Engine 5.8.4 binary distribution, pinned to changelist 58902052 in [toolchain.json](../../../planning/toolchain.json). No separate AI engine edition is required. Epic's official Experimental MCP plugin supplies editor control; Python commandlets and build tools provide independent automation paths. [Epic MCP documentation](https://dev.epicgames.com/documentation/en-us/unreal-engine/unreal-mcp-in-unreal-editor).

The initial content-only project enables MCP, All Toolsets, Python, editor utilities, PCG, and editor Geometry Script. Restrict authoring integrations to Editor targets. The All Toolsets dependency on Game Features requires the documented primary asset scan rule in project configuration. Disable Android File Server for this Windows bootstrap; keep device-network credentials out of source.

Use D3D12/SM6 for normal editor rendering, with an explicit null renderer only for headless checks. The launcher accepts configured engine and cache paths and validates the exact engine build before starting a process. MCP binds to loopback, and editor calls run serially. Shared Codex configuration contains only the local endpoint and timeouts.

All Toolsets is useful for initial capability discovery but increases editor startup dependencies. Narrow enabled toolsets once actual content workflows are established. Experimental tool success is setup evidence, not proof of gameplay correctness. [Validation results and open issues](../../quality/toolchain-validation.md) record the completed installation and current limits, including an engine-owned smoke-test failure.
