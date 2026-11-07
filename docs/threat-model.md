# TraceWeaver Threat Model & Security Posture

TraceWeaver handles untrusted third-party API specifications and historical traffic recordings. This document outlines security boundaries and mitigations.

---

## 1. Untrusted Specification Ingestion
- **Threat**: Malicious OpenAPI specifications containing nested circular `$ref` references designed to cause stack overflow or billion-laughs expansion.
- **Mitigation**: 
  - Recursion depth ceiling limit (max depth 32).
  - Memoized path stack `refStack_` detecting cycles immediately and terminating recursion.
  - File upload size cap (max 10MB per specification).

---

## 2. Server-Side Request Forgery (SSRF) & Replay Safeguards
- **Threat**: Malicious replay configurations targeting internal cloud metadata endpoints (`http://169.254.169.254`) or loopback infrastructure.
- **Mitigation**:
  - Replay targets require explicit whitelist configuration.
  - State-changing HTTP verbs (`POST`, `PUT`, `DELETE`, `PATCH`) are disabled by default (`allowStateChangingReplay: false`).
  - Strict host resolution filtering preventing private subnet routing in production mode.

---

## 3. Sensitive Data Redaction in Request Recordings
- **Threat**: Accidentally logging Authorization headers, bearer tokens, passwords, cookies, or credit card numbers in historical recordings.
- **Mitigation**:
  - Ingress `DataRedactor` automatically intercepts all headers matching `auth`, `token`, `password`, `secret`, `cookie`, `card`, `api_key`.
  - Values are masked to `[REDACTED]` prior to PostgreSQL insertion.
  - Configurable customer redaction rules allow masking specific JSON payload paths.
