package com.example.logic_server.domain.user.service;

import com.example.logic_server.domain.user.dto.RegisterRequest;
import com.example.logic_server.domain.user.dto.UserResponse;
import com.example.logic_server.domain.user.entity.MemberEntity;
import com.example.logic_server.domain.user.repository.MemberRepository;
import lombok.RequiredArgsConstructor;
import org.springframework.security.core.userdetails.UsernameNotFoundException;
import org.springframework.security.crypto.password.PasswordEncoder;
import org.springframework.stereotype.Service;

@Service
@RequiredArgsConstructor
public class MemberService {

    private final MemberRepository memberRepository;
    private final PasswordEncoder passwordEncoder;

    public void register(RegisterRequest request) {
        if (memberRepository.existsByUsername(request.getUsername())) {
            throw new IllegalArgumentException("이미 사용 중인 아이디입니다.");
        }
        MemberEntity member = new MemberEntity();
        member.setUsername(request.getUsername());
        member.setPassword(passwordEncoder.encode(request.getPassword()));
        member.setApiKey(request.getApiKey());
        memberRepository.save(member);
    }

    public UserResponse getUserInfo(String username) {
        MemberEntity member = memberRepository.findByUsername(username)
                .orElseThrow(() -> new UsernameNotFoundException("사용자를 찾을 수 없습니다."));
        return new UserResponse(member.getUsername(), member.getApiKey());
    }
}